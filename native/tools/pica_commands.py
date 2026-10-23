"""Bounded PICA command inspection, with conservative register knowledge.

This is a command decoder, not a GPU emulator. Each header describes one first
parameter, zero or more extra parameters, a four-bit *byte* write mask, and
whether those parameters target successive registers. Packets occupy whole
eight-byte blocks; the final alignment word is padding, of unspecified value.

Only complete, finite float32 vertex shader uniforms 6--8 are interpreted for
the BCH geometry inspector. Float24 vertex uniforms and partial uniform writes
are explicitly unsupported. Other register writes remain available as metadata;
this module does not execute draw commands, fixed attributes or shader code.
"""
from dataclasses import dataclass
import math
import struct


MAX_WORDS = 65536
VSH_FLOATUNIFORM_INDEX = 0x2C0
VSH_FLOATUNIFORM_DATA0 = 0x2C1
VSH_FLOATUNIFORM_DATA7 = 0x2C8
SUPPORTED_UNIFORMS = frozenset((6, 7, 8))


class PICACommandError(ValueError):
    """The declared stream is malformed or exceeds the supported subset."""


@dataclass(frozen=True)
class CommandWrite:
    """One raw parameter write; source_offset addresses its four input bytes."""

    register: int
    value: int
    mask: int
    source_offset: int


@dataclass
class CommandStream:
    """Known state after the stream, plus raw writes in their original order.

    registers contains only values whose four bytes were all written.
    register_values also contains partial values, with unknown bytes zeroed;
    callers must check register_known_masks before interpreting any such bits.
    Masks
    in register_known_masks use the header's four-bit byte-mask convention.
    register_sources contains an offset only when a single full-mask parameter
    establishes the current value; a subsequent partial write removes it.
    uniforms maps supported indices to complete (x, y, z, w) float tuples.
    """

    registers: dict
    register_values: dict
    register_known_masks: dict
    register_sources: dict
    uniforms: dict
    writes: tuple


def decode_commands(data, offset, word_count, *, max_words=MAX_WORDS):
    """Decode exactly word_count little-endian words starting at byte offset.

    The caller must separately confine this extent to its BCH command section.
    No state is inherited from previous streams, and no input bytes are changed.
    A caller may lower max_words, but cannot exceed the module's hard budget.
    Incomplete float vectors are omitted instead of inventing missing values.
    """
    if any(type(value) is not int for value in (offset, word_count, max_words)):
        raise PICACommandError('command extent and word budget must be integers')
    if not 0 <= max_words <= MAX_WORDS or not 0 <= word_count <= max_words:
        raise PICACommandError('command word count exceeds budget')
    if offset < 0 or offset % 4 or offset > len(data):
        raise PICACommandError('invalid or unaligned command offset')
    if word_count > (len(data) - offset) // 4:
        raise PICACommandError('command extent outside input')

    values = {}
    known_masks = {}
    sources = {}
    uniforms = {}
    writes = []
    uniform_component = None
    uniform_values = []

    def apply(write):
        nonlocal uniform_component, uniform_values
        register, value, mask = write.register, write.value, write.mask
        writes.append(write)
        if not mask:
            return
        # Bit 0 writes bits 0--7, bit 1 writes bits 8--15, and so on.
        bits = sum(0xFF << (byte * 8) for byte in range(4) if mask & (1 << byte))
        values[register] = (values.get(register, 0) & ~bits) | (value & bits)
        known_masks[register] = known_masks.get(register, 0) | mask
        if mask == 0xF:
            sources[register] = write.source_offset
        else:
            sources.pop(register, None)

        if register == VSH_FLOATUNIFORM_INDEX:
            if mask != 0xF:
                raise PICACommandError('partial vertex uniform index write unsupported')
            if not value & 0x80000000:
                raise PICACommandError('float24 vertex uniforms unsupported')
            index = value & 0xFF
            if index >= 96:
                raise PICACommandError('vertex uniform index outside 0..95')
            uniform_component = index * 4
            uniform_values = []
        elif VSH_FLOATUNIFORM_DATA0 <= register <= VSH_FLOATUNIFORM_DATA7:
            if mask != 0xF:
                raise PICACommandError('partial vertex uniform data write unsupported')
            if uniform_component is None:
                raise PICACommandError('vertex uniform data precedes index initialization')
            index, component = divmod(uniform_component, 4)
            if index >= 96:
                raise PICACommandError('vertex uniform upload exceeds index 95')
            if index in SUPPORTED_UNIFORMS:
                number = struct.unpack('<f', struct.pack('<I', value))[0]
                if not math.isfinite(number):
                    raise PICACommandError('non-finite vertex geometry uniform')
                if component == 0:
                    # Do not expose stale state if this overwrite remains partial.
                    uniforms.pop(index, None)
                    uniform_values = []
                uniform_values.append(number)
                if component == 3:
                    # PICA streams float32 components in w, z, y, x order.
                    uniforms[index] = tuple(reversed(uniform_values))
            uniform_component += 1

    cursor = 0
    while cursor < word_count:
        if word_count - cursor < 2:
            raise PICACommandError('truncated command parameter/header pair')
        packet_at = offset + cursor * 4
        first, header = struct.unpack_from('<II', data, packet_at)
        register = header & 0xFFFF
        mask = (header >> 16) & 0xF
        extra = (header >> 20) & 0x7FF
        consecutive = bool(header & 0x80000000)
        packet_words = 2 + extra
        padded_words = packet_words + (packet_words & 1)
        if packet_words > word_count - cursor:
            raise PICACommandError('command parameters exceed declared extent')
        if padded_words > word_count - cursor:
            raise PICACommandError('missing command alignment padding')
        if register > 0x3FF or (consecutive and register + extra > 0x3FF):
            raise PICACommandError('command register outside 0x000..0x3ff')

        apply(CommandWrite(register, first, mask, packet_at))
        for parameter in range(extra):
            source = packet_at + 8 + parameter * 4
            value = struct.unpack_from('<I', data, source)[0]
            target = register + parameter + 1 if consecutive else register
            apply(CommandWrite(target, value, mask, source))
        cursor += padded_words

    registers = {register: value for register, value in values.items()
                 if known_masks[register] == 0xF}
    return CommandStream(registers, values, known_masks, sources, uniforms, tuple(writes))
