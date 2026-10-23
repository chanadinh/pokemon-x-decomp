#include "assets/bclim.h"
#include "assets/garc.h"
#include "assets/lz.h"

#include <SDL.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MEMBER_LIMIT = 64 * 1024 * 1024 };

struct options {
    const char *romfs, *archive, *screenshot, *capture_frame;
    uint32_t entry, subentry, frames;
    int have_entry, have_subentry, demo, inspect, software, self_test;
};

struct view {
    double pan_x, pan_y, zoom;
    uint32_t image_width, image_height;
    int width, height, running;
};

static void usage(FILE *stream, const char *program)
{
    fprintf(stream,
        "Usage: %s (--demo | --romfs PATH --archive RELATIVE/PATH --entry N) [options]\n"
        "  --subentry N          GARC bitmap subentry, 0..31 (default 0)\n"
        "  --inspect             Decode and print metadata without opening a window\n"
        "  --screenshot FILE.bmp Export the decoded image; destination must be new\n"
        "  --capture-frame FILE.bmp Save first window frame; destination must be new\n"
        "  --frames N            Exit after N rendered frames; N must be positive\n"
        "  --software            Use the SDL software renderer\n"
        "  --self-test-input     Test injected input; defaults to 3 rendered frames\n"
        "  --help                Show this help\n"
        "Controls: arrow keys pan; + / - zoom; R or Home resets; Escape exits.\n"
        "This is an asset viewer, not gameplay. --demo uses a synthetic image.\n",
        program);
}

static int number(const char *text, uint32_t *out)
{
    if (*text == '\0') return 0;
    for (const char *p = text; *p != '\0'; ++p)
        if (*p < '0' || *p > '9') return 0;
    errno = 0;
    char *end;
    const uintmax_t value = strtoumax(text, &end, 10);
    if (errno != 0 || *end != '\0' || value > UINT32_MAX) return 0;
    *out = (uint32_t)value;
    return 1;
}

static int relative_archive(const char *path)
{
    if (*path == '\0' || *path == '/') return 0;
    const char *component = path;
    for (const char *p = path;; ++p) {
        if (*p == '\\' || *p == ':') return 0;
        if (*p != '/' && *p != '\0') continue;
        const size_t length = (size_t)(p - component);
        if (length == 0 || (length == 1 && component[0] == '.') ||
            (length == 2 && component[0] == '.' && component[1] == '.')) return 0;
        if (*p == '\0') return 1;
        component = p + 1;
    }
}

static int parse_options(int argc, char **argv, struct options *options)
{
    for (int i = 1; i < argc; ++i) {
        const char *argument = argv[i];
        if (strcmp(argument, "--help") == 0) { usage(stdout, argv[0]); return 2; }
        if (strcmp(argument, "--demo") == 0) { options->demo = 1; continue; }
        if (strcmp(argument, "--inspect") == 0) { options->inspect = 1; continue; }
        if (strcmp(argument, "--software") == 0) { options->software = 1; continue; }
        if (strcmp(argument, "--self-test-input") == 0) { options->self_test = 1; continue; }
        if (strcmp(argument, "--romfs") != 0 && strcmp(argument, "--archive") != 0 &&
            strcmp(argument, "--entry") != 0 && strcmp(argument, "--subentry") != 0 &&
            strcmp(argument, "--frames") != 0 && strcmp(argument, "--screenshot") != 0 &&
            strcmp(argument, "--capture-frame") != 0) {
            fprintf(stderr, "[asset-viewer] Unknown option: %s\n", argument);
            return 0;
        }
        if (++i == argc || argv[i][0] == '\0') {
            fprintf(stderr, "[asset-viewer] Missing value for %s\n", argument);
            return 0;
        }
        const char *value = argv[i];
        if (strcmp(argument, "--romfs") == 0) options->romfs = value;
        else if (strcmp(argument, "--archive") == 0) options->archive = value;
        else if (strcmp(argument, "--screenshot") == 0) options->screenshot = value;
        else if (strcmp(argument, "--capture-frame") == 0) options->capture_frame = value;
        else {
            uint32_t parsed;
            if (!number(value, &parsed) ||
                (strcmp(argument, "--frames") == 0 && parsed == 0) ||
                (strcmp(argument, "--subentry") == 0 && parsed > 31)) {
                fprintf(stderr, "[asset-viewer] Invalid value for %s: %s\n", argument, value);
                return 0;
            }
            if (strcmp(argument, "--entry") == 0) {
                options->entry = parsed; options->have_entry = 1;
            } else if (strcmp(argument, "--subentry") == 0) {
                options->subentry = parsed; options->have_subentry = 1;
            } else options->frames = parsed;
        }
    }
    if (options->demo) {
        if (options->romfs || options->archive || options->have_entry || options->have_subentry) {
            fprintf(stderr, "[asset-viewer] --demo cannot be combined with archive selection.\n");
            return 0;
        }
    } else if (!options->romfs || !options->archive || !options->have_entry ||
               !relative_archive(options->archive)) {
        fprintf(stderr, "[asset-viewer] Supply --romfs, a relative --archive without dot components, and --entry; or use --demo.\n");
        return 0;
    }
    if (options->inspect && (options->frames || options->self_test || options->capture_frame)) {
        fprintf(stderr, "[asset-viewer] --inspect cannot render/capture frames or test window input.\n");
        return 0;
    }
    if (options->self_test && options->frames == 0) options->frames = 3;
    return 1;
}

static int demo_image(struct px_bclim_image *image)
{
    image->width = 256;
    image->height = 192;
    image->format = UINT32_MAX;
    image->rgba = malloc((size_t)image->width * image->height * 4);
    if (!image->rgba) return 0;
    for (uint32_t y = 0; y < image->height; ++y) {
        for (uint32_t x = 0; x < image->width; ++x) {
            uint8_t *pixel = image->rgba + ((size_t)y * image->width + x) * 4;
            pixel[0] = (uint8_t)x;
            pixel[1] = (uint8_t)(y * 255 / (image->height - 1));
            pixel[2] = ((x / 16 + y / 16) & 1) ? 220 : 48;
            pixel[3] = (x < 16 || y < 16) ? 128 : 255;
        }
    }
    return 1;
}

static int load_image(const struct options *options, struct px_bclim_image *image)
{
    if (options->demo) {
        if (!demo_image(image)) {
            fprintf(stderr, "[asset-viewer] Cannot allocate the synthetic image.\n");
            return 0;
        }
        printf("source=synthetic-demo width=%" PRIu32 " height=%" PRIu32 " pixels=RGBA8\n",
               image->width, image->height);
        return 1;
    }
    const size_t root_length = strlen(options->romfs), archive_length = strlen(options->archive);
    if (root_length > SIZE_MAX - archive_length - 2) return 0;
    char *path = malloc(root_length + archive_length + 2);
    if (!path) { fprintf(stderr, "[asset-viewer] Cannot allocate the archive path.\n"); return 0; }
    memcpy(path, options->romfs, root_length);
    path[root_length] = '/';
    memcpy(path + root_length + 1, options->archive, archive_length + 1);

    struct px_garc *archive = NULL;
    uint8_t *member = NULL, *decoded = NULL;
    size_t member_size = 0, decoded_size = 0;
    char error[256];
    if (!px_garc_open(path, &archive, error, sizeof error)) {
        fprintf(stderr, "[asset-viewer] Archive %s: %s\n", path, error);
        free(path);
        return 0;
    }
    free(path);
    const size_t member_count = px_garc_count(archive);
    const int read_ok = px_garc_read(archive, options->entry, options->subentry,
                                    MEMBER_LIMIT, &member, &member_size, error, sizeof error);
    px_garc_close(archive);
    if (!read_ok) {
        fprintf(stderr, "[asset-viewer] Entry %" PRIu32 "/%" PRIu32 ": %s\n",
                options->entry, options->subentry, error);
        return 0;
    }
    enum px_bclim_result result = px_bclim_decode(member, member_size, image);
    const char *compression = "none";
    if (result != PX_BCLIM_OK && member_size != 0 && (member[0] == 0x10 || member[0] == 0x11)) {
        compression = member[0] == 0x10 ? "LZ10" : "LZ11";
        if (!px_lz_decode(member, member_size, MEMBER_LIMIT, &decoded, &decoded_size,
                          error, sizeof error)) {
            fprintf(stderr, "[asset-viewer] Raw BCLIM: %s; %s decode: %s\n",
                    px_bclim_result_string(result), compression, error);
            free(member);
            return 0;
        }
        result = px_bclim_decode(decoded, decoded_size, image);
    }
    if (result != PX_BCLIM_OK) {
        fprintf(stderr, "[asset-viewer] BCLIM decode: %s\n", px_bclim_result_string(result));
        free(decoded);
        free(member);
        return 0;
    }
    printf("archive=%s entry=%" PRIu32 " subentry=%" PRIu32 " members=%zu member_bytes=%zu compression=%s decoded_bytes=%zu width=%" PRIu32 " height=%" PRIu32 " format=%" PRIu32 " pixels=RGBA8\n",
           options->archive, options->entry, options->subentry, member_count, member_size,
           compression, decoded ? decoded_size : member_size, image->width, image->height, image->format);
    free(decoded);
    free(member);
    return 1;
}

static void reset_view(struct view *view)
{
    const int width = view->width > 64 ? view->width - 64 : 1;
    const int height = view->height > 64 ? view->height - 64 : 1;
    double zoom = (double)width / view->image_width;
    const double vertical = (double)height / view->image_height;
    if (vertical < zoom) zoom = vertical;
    if (zoom > 8.0) zoom = 8.0;
    if (zoom < 0.03125) zoom = 0.03125;
    view->zoom = zoom;
    view->pan_x = view->pan_y = 0;
}

static void handle_event(struct view *view, const SDL_Event *event)
{
    if (event->type == SDL_QUIT) view->running = 0;
    if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED &&
        event->window.data1 > 0 && event->window.data2 > 0) {
        view->width = event->window.data1;
        view->height = event->window.data2;
    }
    if (event->type != SDL_KEYDOWN) return;
    switch (event->key.keysym.sym) {
    case SDLK_ESCAPE: view->running = 0; break;
    case SDLK_LEFT: view->pan_x -= 32; break;
    case SDLK_RIGHT: view->pan_x += 32; break;
    case SDLK_UP: view->pan_y -= 32; break;
    case SDLK_DOWN: view->pan_y += 32; break;
    case SDLK_PLUS: case SDLK_EQUALS: case SDLK_KP_PLUS: view->zoom *= 1.25; break;
    case SDLK_MINUS: case SDLK_KP_MINUS: view->zoom /= 1.25; break;
    case SDLK_r: case SDLK_HOME: reset_view(view); break;
    default: break;
    }
    if (view->zoom < 0.03125) view->zoom = 0.03125;
    if (view->zoom > 64) view->zoom = 64;
    if (view->pan_x < -1000000) view->pan_x = -1000000;
    if (view->pan_x > 1000000) view->pan_x = 1000000;
    if (view->pan_y < -1000000) view->pan_y = -1000000;
    if (view->pan_y > 1000000) view->pan_y = 1000000;
}

static int injected_event(struct view *view, SDL_Event *event)
{
    if (SDL_PushEvent(event) != 1) return 0;
    SDL_Event received;
    while (SDL_PollEvent(&received)) handle_event(view, &received);
    return 1;
}

static int test_input(struct view *view)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) handle_event(view, &event);
    const struct view saved = *view;
    reset_view(view);
    const double starting_zoom = view->zoom;
    memset(&event, 0, sizeof event);
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_RIGHT;
    if (!injected_event(view, &event) || view->pan_x != 32) return 0;
    event.key.keysym.sym = SDLK_UP;
    if (!injected_event(view, &event) || view->pan_y != -32) return 0;
    event.key.keysym.sym = SDLK_PLUS;
    if (!injected_event(view, &event) || view->zoom <= starting_zoom) return 0;
    event.key.keysym.sym = SDLK_MINUS;
    if (!injected_event(view, &event) || view->zoom - starting_zoom > 1e-12 ||
        view->zoom - starting_zoom < -1e-12) return 0;
    event.key.keysym.sym = SDLK_r;
    if (!injected_event(view, &event) || view->pan_x != 0 || view->pan_y != 0 ||
        view->zoom != starting_zoom) return 0;
    memset(&event, 0, sizeof event);
    event.type = SDL_WINDOWEVENT;
    event.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
    event.window.data1 = 640; event.window.data2 = 480;
    if (!injected_event(view, &event) || view->width != 640 || view->height != 480) return 0;
    memset(&event, 0, sizeof event);
    event.type = SDL_KEYDOWN; event.key.keysym.sym = SDLK_ESCAPE;
    if (!injected_event(view, &event) || view->running != 0) return 0;
    *view = saved;
    fprintf(stderr, "[asset-viewer] Input self-test passed: pan, zoom, reset, resize, Escape.\n");
    return 1;
}

static int save_new_bmp(SDL_Surface *surface, const char *path)
{
    /* Exclusive creation protects existing input/output files and their
     * hardlink or symlink aliases without a path-check/write race. */
    FILE *file = fopen(path, "wbx");
    if (!file) {
        SDL_SetError("Cannot create new export %s: %s", path, strerror(errno));
        return 0;
    }
    SDL_RWops *output = SDL_RWFromFP(file, SDL_TRUE);
    if (!output) { fclose(file); return 0; }
    return SDL_SaveBMP_RW(surface, output, 1) == 0;
}

static int capture_frame(SDL_Renderer *renderer, const char *path)
{
    int width, height;
    if (SDL_GetRendererOutputSize(renderer, &width, &height) != 0) return 0;
    SDL_Surface *frame = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!frame) return 0;
    const int ok = SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32,
                                        frame->pixels, frame->pitch) == 0 &&
                   save_new_bmp(frame, path);
    SDL_FreeSurface(frame);
    if (ok) fprintf(stderr, "[asset-viewer] Saved rendered frame %dx%d: %s\n", width, height, path);
    return ok;
}

static int render_image(SDL_Renderer *renderer, SDL_Texture *texture, const struct view *view,
                         const char *capture_path)
{
    if (SDL_SetRenderDrawColor(renderer, 31, 35, 43, 255) != 0 || SDL_RenderClear(renderer) != 0)
        return 0;
    if (SDL_SetRenderDrawColor(renderer, 42, 47, 56, 255) != 0) return 0;
    for (int y = 0; y < view->height; y += 32) {
        for (int x = 0; x < view->width; x += 32) {
            if (((x / 32 + y / 32) & 1) != 0) continue;
            const SDL_Rect tile = {x, y, 32, 32};
            if (SDL_RenderFillRect(renderer, &tile) != 0) return 0;
        }
    }
    int width = (int)(view->image_width * view->zoom);
    int height = (int)(view->image_height * view->zoom);
    if (width < 1) width = 1;
    if (height < 1) height = 1;
    const SDL_Rect destination = {
        (int)(((double)view->width - width) / 2 + view->pan_x),
        (int)(((double)view->height - height) / 2 + view->pan_y), width, height
    };
    if (SDL_RenderCopy(renderer, texture, NULL, &destination) != 0) return 0;
    if (capture_path && !capture_frame(renderer, capture_path)) return 0;
    SDL_RenderPresent(renderer);
    return 1;
}

static int run_window(const struct options *options, SDL_Surface *surface,
                       const struct px_bclim_image *image)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "[asset-viewer] SDL initialization: %s\n", SDL_GetError());
        return 0;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Window *window = SDL_CreateWindow("Pokémon X Asset Viewer — arrows: pan | +/-: zoom | R: reset | Esc: exit",
                                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           960, 640, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    int ok = 0;
    if (!window) goto done;
    if (!options->software)
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) goto done;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture || SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0) goto done;

    SDL_RendererInfo renderer_info;
    if (SDL_GetRendererInfo(renderer, &renderer_info) != 0) goto done;
    fprintf(stderr, "[asset-viewer] Video=%s renderer=%s image=%" PRIu32 "x%" PRIu32 ". This viewer does not run gameplay.\n",
            SDL_GetCurrentVideoDriver(), renderer_info.name, image->width, image->height);
    struct view view = {0, 0, 1, image->width, image->height, 960, 640, 1};
    SDL_GetWindowSize(window, &view.width, &view.height);
    reset_view(&view);
    if (options->self_test && !test_input(&view)) {
        fprintf(stderr, "[asset-viewer] Injected input self-test failed.\n");
        goto done;
    }
    const Uint64 frequency = SDL_GetPerformanceFrequency();
    if (frequency == 0) goto done;
    const Uint64 start = SDL_GetPerformanceCounter();
    uint32_t frames = 0;
    while (view.running) {
        const Uint64 frame_start = SDL_GetPerformanceCounter();
        SDL_Event event;
        while (SDL_PollEvent(&event)) handle_event(&view, &event);
        if (!view.running) break;
        if (SDL_GetRendererOutputSize(renderer, &view.width, &view.height) != 0 ||
            !render_image(renderer, texture, &view, frames == 0 ? options->capture_frame : NULL)) goto done;
        ++frames;
        if (options->frames != 0 && frames >= options->frames) break;
        const double elapsed = (double)(SDL_GetPerformanceCounter() - frame_start) / (double)frequency;
        if (elapsed < 0.016) SDL_Delay((Uint32)((0.016 - elapsed) * 1000));
    }
    fprintf(stderr, "[asset-viewer] Exit after %" PRIu32 " rendered frames, %.3f seconds.\n",
            frames, (double)(SDL_GetPerformanceCounter() - start) / (double)frequency);
    ok = 1;
done:
    if (!ok) fprintf(stderr, "[asset-viewer] Runtime error: %s\n", SDL_GetError());
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
    return ok;
}

int main(int argc, char **argv)
{
    struct options options = {0};
    const int parsed = parse_options(argc, argv, &options);
    if (parsed == 2) return EXIT_SUCCESS;
    if (!parsed) { usage(stderr, argv[0]); return EXIT_FAILURE; }
    struct px_bclim_image image = {0};
    if (!load_image(&options, &image)) return EXIT_FAILURE;
    if (image.width == 0 || image.height == 0 || image.width > 4096 || image.height > 4096) {
        fprintf(stderr, "[asset-viewer] Unsupported decoded image dimensions.\n");
        px_bclim_free(&image);
        return EXIT_FAILURE;
    }
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(image.rgba, (int)image.width,
        (int)image.height, 32, (int)(image.width * 4), SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        fprintf(stderr, "[asset-viewer] Image surface: %s\n", SDL_GetError());
        px_bclim_free(&image);
        return EXIT_FAILURE;
    }
    int ok = 1;
    if (options.screenshot) {
        if (!save_new_bmp(surface, options.screenshot)) {
            fprintf(stderr, "[asset-viewer] Image export %s: %s\n", options.screenshot, SDL_GetError());
            ok = 0;
        } else fprintf(stderr, "[asset-viewer] Saved decoded image at original size: %s\n", options.screenshot);
    }
    if (ok && !options.inspect) ok = run_window(&options, surface, &image);
    SDL_FreeSurface(surface);
    px_bclim_free(&image);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
