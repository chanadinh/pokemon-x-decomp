Native skeletal animation of the player character, 2026-10-24

The player model of ../bch-player now moves: its GF1 motion pack is decoded,
evaluated per frame and applied to the skeleton, and in the walkable room the
player plays a walk cycle while gliding from cell to cell. Tracked files contain
independently authored source, synthetic tests and metadata only; the
documentation PNGs are recorded in docs/images/README.md.

Native preview

native/build/pokemon_model_viewer --romfs /path/to/romfs \
  --archive a/0/4/1 --entry 165 --section 1 --triangle-list --textured --combiners \
  --texture-archive a/0/1/3 --texture-entry 75 --texture-section 11 \
  --environment-archive a/0/0/3 --environment-entry 137 --environment-section 0 \
  --walk --player-archive a/0/2/1 --player-entry 2 --player-section 0 \
  --motion-section 1 --motion 1 --walk-motion 3

The model alone, looping one motion: --archive a/0/2/1 --entry 2 --section 0
--skinned --triangle-list --textured --combiners with the same archive as
--texture-archive, then --motion-section 1 --motion N.

Options

--motion-section N  GF1 pack: this MM section of the skinned model's entry (the
                    player's entry with --player-archive). Required with --motion.
--motion N          Slot N (1..). It loops: one motion frame per two rendered
                    frames, period = the motion's frame count (frame N repeats
                    frame 0). Requires --skinned or --player-archive.
--motion-frame N    Start frame. With --walk-motion it indexes the walk cycle,
                    which the last --steps move plays as display starts (if that
                    move was not blocked), so captures can show any phase.
--walk-motion N     With --player-archive: slot N plays once from frame 0 after
                    each marker step while the player glides linearly from the
                    previous cell center to the new one over the cycle; --motion
                    plays otherwise.
--inspect prints a motion_section= line (frames, pack slot and motion counts,
skeleton sizes, matched bones, world-space bones, consumed bytes, posed bounds
at the start frame, attachment and its rest residual) and a walk_motion= line.

The pack (assets/gf_motion.h)

a/0/2/1 entry 2, MM section 1: 820,864 bytes, 173 slots. Format as read by
SPICA bd29a78 (GF1MotionPack, GF1Motion, GF1MotBone): slot 0 a motion skeleton
of 42 bones (implicit "Origin", then parent/flags/children, names, translation
and quaternion), slots 1..172 motions or empty (offset 0). Each motion: code and
frame counts; 3-bit codes eight per 24-bit word; one key-frame list per code 6
or 7 (u8, or u16 aligned to 2 when frames > 255) with implicit first frame 0
and last frame = frame count; 4-byte alignment; values. After two header codes,
codes address nine channels per bone (TX TY TZ RX RY RZ SX SY SZ): 1 skips
three channels, 0 = 0, 2 = +pi/2, 3 = pi, 4 = -pi/2, 5 one float, 6 linear,
7 Hermite (value and slope per key). The decoder bounds every read, rejects
non-finite values, extra or duplicate channels and unused key lists, and
requires each motion to end at or before the next nonzero slot.
tr0002: 118 motions, 54 empty slots, 95,232 keys; every motion ends exactly at
the next slot (all 118). Channels: 8,206 Hermite, 15 linear, 5,636 constant.

Evaluation follows SPICA H3DFloatKeyFrameGroup.GetFrameValue and Herp, with
Hermite slopes divided by 30 as GF1Motion.SetKeyFrameGroup does (SPICA notes
the factor was found by trial; it is not traced in the game's code). Frames are
integers here; there is no inter-frame interpolation.

Posing (render/skin.h)

px_skin_motion_locals: every H3D bone starts from its stored scale, Euler
rotation and translation; a motion bone of the same name (42 of 45 match; the
unmatched are the model root, "model" and "glass") overrides the channels it
has. LFoot and RFoot carry flag 8 (world space). Their channels are converted
as SPICA's ToH3DSkeletalAnimation does: the target translation is brought into
the parent's frame (ancestor chain of rotation and translation, without scale),
the parent (LLeg/RLeg) receives the rotation from its own translation toward
that target unless the motion animates its rotation, and the foot's translation
becomes the target with that rotation removed. SPICA calls this conversion
improvable; the game's own solver is not traced. Bone worlds compose as in the
rest pose (px_skin_world_from_locals) and every index is posed into the viewer's
arrays each frame (px_skin_pose_indices); the model keeps its source arrays.

Attachment policy: the rigid glasses are skinned to "glass", a child of the
root "model" that no motion animates, so they stayed in place while the head
moved. glass's rest world equals that of the head locator loc_acchat within
7.31e-5 (every matrix element), so the viewer re-parents glass onto loc_acchat
(px_skin_attach: world = target world x inverse target rest x child rest) when
both bones exist, glass is not in the motion and the residual is at most 1e-4.
The game's accessory attachment is not traced; this is an inspection rule.

Which motions

Identified by rendering, not by tracing the game's motion table: slot 1 is a
one-frame standing pose, slot 3 a 14-frame walk cycle, slot 4 a 16-frame run;
slots 31 onward include seated poses. --walk-motion 3 with --motion 1 is the
walking preview. Which slots the game plays, and its playback rate, are not
traced.

Independent comparison (verify_motion.py)

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror -D_DEFAULT_SOURCE \
  native/assets/bch_model.c native/assets/gf_motion.c native/render/skin.c \
  native/reproduction/bch-motion/dump_motion.c -lm -o native/build/dump_motion
python3 -I -B native/reproduction/bch-motion/verify_motion.py --romfs /path/to/romfs \
  --bridge native/build/dump_motion --viewer native/build/pokemon_model_viewer \
  --output-dir outputs/new-motion-check

It extracts the model and pack itself, parses the pack itself (requiring exact
consumption of every motion), and compares the skeleton, every motion's frame
count, byte range, channel kinds and all 95,232 keys bit for bit. For slots 1,
3 and 4 at every integer frame (2 + 15 + 17 frames) it evaluates the 4,110
present channel values in emulated float32 in the C expression order (all
bits identical), builds local transforms with its own world-space conversion
and attachment (largest difference 2.6e-15) and poses all 6,099 indices with
../bch-player/verify_player.py's rules on these worlds (positions and normals
identical). Viewer: inspect lines and capture hashes of five standalone frames
and five walk-cycle phases, in reference-comparison.json.

The ../bch-player bridge build now needs native/assets/gf_motion.c as well,
because render/skin.c uses the motion evaluator; its recorded report is
reproduced unchanged with that addition.

Tests

assets/test_gf_motion.c: a hand-built pack (skeleton with a world-space bone,
an empty slot, a short motion using every code, a 300-frame motion with u16
keys and the alignment byte): decoded fields, consumed ranges, constants,
linear and Hermite values, the slope scale, and rejections (truncation, a
motion running past the next slot, offsets beyond the pack, a forward parent,
the slot cap, extra codes, a non-finite key). render/test_skin.c: hand-worked
world-space conversion (Euler pi/2 on the parent, translation (0, 5, 0) on the
child, the child's world at the target), the kept parent rotation, name
mapping and fallback, frame clamping, rejection without a parent, attachment
and posing into caller buffers. tests/test_bch_motion_viewer.py: a synthetic
skinned BCH and GF1 pack in one MM pack: posed bounds per frame, looping,
captures, the walk cycle's three phases, a blocked last step, option errors,
an empty slot, a frame past the end, a pack that does not decode (no video),
and help text.

Limits

Integer frames only, advanced per rendered frame (so playback slows when
rendering is slow). The walk cycle, its glide and the idle pose are inspection
choices; the game's player controller, motion selection, blending, playback
rate and root motion are not traced. The male player, outfits, eye/mouth
material animations (MM section 2) and the vertex shader's normal quaternion
remain unexecuted. The model's front is still taken to be +Z.
