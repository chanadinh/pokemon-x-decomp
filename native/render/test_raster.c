#include "raster.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "raster:%d: %s\n", __LINE__, #c); exit(1); } } while (0)
#define OK(c) CHECK((c) == PX_RASTER_OK)

static const uint8_t black[4] = {0, 0, 0, 255};
static const uint8_t zero[4] = {0, 0, 0, 0};
static const uint8_t white[4] = {255, 255, 255, 255};
static const uint8_t red[4] = {255, 0, 0, 255};
static const uint8_t green[4] = {0, 255, 0, 255};
static const uint8_t blue[4] = {0, 0, 255, 255};

static struct px_raster_vertex vertex(double x, double y, double u, double v)
{
    struct px_raster_vertex result = {x, y, 0, u, v, {1, 1, 1, 1}};
    return result;
}

static void triangle(struct px_raster_vertex p[3], double u, double v)
{
    p[0] = vertex(0, 0, u, v);
    p[1] = vertex(8, 0, u, v);
    p[2] = vertex(0, 8, u, v);
}

static void color(struct px_raster_vertex p[3], const uint8_t rgba[4], double z)
{
    for (unsigned i = 0; i < 3; ++i) {
        p[i].z = z;
        for (unsigned c = 0; c < 4; ++c) p[i].rgba[c] = rgba[c] / 255.0;
    }
}

static void pixel(const struct px_raster_target *t, unsigned x, unsigned y, const uint8_t expected[4])
{
    const uint8_t *got = t->rgba + ((size_t)y * t->width + x) * 4;
    if (memcmp(got, expected, 4)) {
        fprintf(stderr, "pixel %u,%u: got %u,%u,%u,%u expected %u,%u,%u,%u\n",
                x, y, got[0], got[1], got[2], got[3], expected[0], expected[1], expected[2], expected[3]);
        exit(1);
    }
}

static void dimensions(void)
{
    struct px_raster_target t;
    CHECK(px_raster_create(0, 1, &t) == PX_RASTER_INVALID_ARGUMENT);
    CHECK(!t.rgba && !t.depth && !t.width);
    CHECK(px_raster_create(4097, 1, &t) == PX_RASTER_LIMIT_EXCEEDED);
    CHECK(px_raster_create(4096, 1025, &t) == PX_RASTER_LIMIT_EXCEEDED);
    CHECK(px_raster_create(1, 1, NULL) == PX_RASTER_INVALID_ARGUMENT);
    OK(px_raster_create(2, 3, &t));
    OK(px_raster_clear(&t, blue, INFINITY));
    for (unsigned y = 0; y < 3; ++y) for (unsigned x = 0; x < 2; ++x) {
        pixel(&t, x, y, blue);
        CHECK(t.depth[y * 2 + x] == INFINITY);
    }
    CHECK(px_raster_clear(&t, red, NAN) == PX_RASTER_INVALID_ARGUMENT);
    CHECK(px_raster_clear(&t, red, -INFINITY) == PX_RASTER_INVALID_ARGUMENT);
    pixel(&t, 1, 2, blue);
    px_raster_free(&t);
    CHECK(!t.rgba && !t.depth && !t.width && !t.height && !t.candidate_pixels);
    px_raster_free(&t);
    px_raster_free(NULL);
}

static void uv_and_interpolation(void)
{
    const uint8_t texture[16] = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,255,255};
    const struct px_raster_image image = {2, 2, texture, sizeof(texture)};
    const struct px_raster_sampler sampler = {PX_RASTER_CLAMP, PX_RASTER_CLAMP, PX_RASTER_NEAREST};
    struct px_raster_state state = px_raster_default_state();
    struct px_raster_target t;
    struct px_raster_vertex p[3];
    OK(px_raster_create(2, 2, &t));
    OK(px_raster_clear(&t, black, INFINITY));
    p[0] = vertex(0,0,0,0); p[1] = vertex(2,0,1,0); p[2] = vertex(0,2,0,1);
    OK(px_raster_draw(&t, p, &image, &sampler, &state));
    p[0] = vertex(2,0,1,0); p[1] = vertex(2,2,1,1); p[2] = vertex(0,2,0,1);
    OK(px_raster_draw(&t, p, &image, &sampler, &state));
    pixel(&t,0,0,red); pixel(&t,1,0,green); pixel(&t,0,1,blue); pixel(&t,1,1,white);
    OK(px_raster_clear(&t, black, INFINITY));
    p[0] = vertex(0,0,0,0); p[1] = vertex(2,0,0,0); p[2] = vertex(0,2,0,0);
    for (unsigned i=0; i<3; ++i) for (unsigned c=0; c<3; ++c) p[i].rgba[c] = i == c;
    p[0].z=2; p[1].z=6; p[2].z=10;
    OK(px_raster_draw(&t,p,NULL,NULL,&state));
    /* At (.5,.5), weights are exactly 1/2,1/4,1/4. */
    pixel(&t,0,0,(const uint8_t[]){128,64,64,255});
    CHECK(t.depth[0] == 5);
    px_raster_free(&t);
}

static void sampling(void)
{
    const uint8_t texture[16] = {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,255,255};
    const struct px_raster_image row = {4,1,texture,sizeof(texture)};
    const struct px_raster_image square = {2,2,texture,sizeof(texture)};
    struct px_raster_sampler sampler = {PX_RASTER_CLAMP,PX_RASTER_CLAMP,PX_RASTER_NEAREST};
    struct px_raster_state state = px_raster_default_state();
    struct px_raster_vertex p[3];
    struct px_raster_target t;
    const double values[] = {-0.375, 1.375, -1.375, 2.375, 1.0};
    const unsigned expected[3][5] = {{0,3,0,3,3},{2,1,2,1,0},{1,2,2,1,3}};
    OK(px_raster_create(1,1,&t));
    for (unsigned wrap=0; wrap<3; ++wrap) for (unsigned i=0; i<5; ++i) {
        sampler.wrap_u=(enum px_raster_wrap)wrap;
        triangle(p,values[i],0.5);
        OK(px_raster_clear(&t,black,INFINITY));
        OK(px_raster_draw(&t,p,&row,&sampler,&state));
        pixel(&t,0,0,texture + expected[wrap][i]*4);
    }
    sampler.wrap_u=PX_RASTER_CLAMP;
    sampler.filter=PX_RASTER_LINEAR;
    triangle(p,.5,.5);
    OK(px_raster_clear(&t,black,INFINITY));
    OK(px_raster_draw(&t,p,&square,&sampler,&state));
    pixel(&t,0,0,(const uint8_t[]){128,128,128,255});
    triangle(p,.25,.25);
    OK(px_raster_clear(&t,black,INFINITY));
    OK(px_raster_draw(&t,p,&square,&sampler,&state));
    pixel(&t,0,0,red);
    sampler.wrap_u=PX_RASTER_REPEAT;
    triangle(p,0,.25);
    OK(px_raster_clear(&t,black,INFINITY));
    OK(px_raster_draw(&t,p,&square,&sampler,&state));
    pixel(&t,0,0,(const uint8_t[]){128,128,0,255});
    sampler.wrap_u=PX_RASTER_MIRROR;
    OK(px_raster_clear(&t,black,INFINITY));
    OK(px_raster_draw(&t,p,&square,&sampler,&state));
    pixel(&t,0,0,red);
    /* V uses the same wrap rules, with a deliberately asymmetric texture. */
    sampler.wrap_v=PX_RASTER_REPEAT; sampler.filter=PX_RASTER_NEAREST;
    triangle(p,.25,-.25);
    OK(px_raster_clear(&t,black,INFINITY));
    OK(px_raster_draw(&t,p,&square,&sampler,&state));
    pixel(&t,0,0,blue);
    sampler.wrap_v=PX_RASTER_MIRROR;
    OK(px_raster_clear(&t,black,INFINITY));
    OK(px_raster_draw(&t,p,&square,&sampler,&state));
    pixel(&t,0,0,red);
    px_raster_free(&t);
}

static void alpha_blend(void)
{
    struct px_raster_target t;
    struct px_raster_vertex p[3];
    struct px_raster_state state=px_raster_default_state();
    const uint8_t half_red[4]={255,0,0,128};
    OK(px_raster_create(1,1,&t)); triangle(p,0,0); color(p,half_red,1);
    OK(px_raster_clear(&t,blue,5));
    state.alpha_compare=PX_RASTER_GREATER; state.alpha_reference=128;
    OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,blue); CHECK(t.depth[0]==5);
    state.alpha_compare=PX_RASTER_GEQUAL; state.blend=PX_RASTER_SOURCE_ALPHA;
    OK(px_raster_draw(&t,p,NULL,NULL,&state));
    pixel(&t,0,0,(const uint8_t[]){128,0,127,191}); CHECK(t.depth[0]==1);
    state.blend=PX_RASTER_SOURCE_OVER;
    OK(px_raster_clear(&t,blue,5));
    OK(px_raster_draw(&t,p,NULL,NULL,&state));
    pixel(&t,0,0,(const uint8_t[]){128,0,127,255});
    OK(px_raster_clear(&t,zero,5));
    OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,half_red);
    OK(px_raster_clear(&t,(const uint8_t[]){0,0,255,128},5));
    OK(px_raster_draw(&t,p,NULL,NULL,&state));
    pixel(&t,0,0,(const uint8_t[]){170,0,85,192});
    /* Alpha comparison is on a rounded byte, so exactly 0.5 passes >=128. */
    for(unsigned i=0;i<3;++i)p[i].rgba[3]=.5;
    state.blend=PX_RASTER_REPLACE;
    OK(px_raster_clear(&t,zero,5));
    OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,half_red);
    color(p,half_red,1);
    state.color_write_mask=5; state.blend=PX_RASTER_REPLACE;
    OK(px_raster_clear(&t,green,5));
    OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,(const uint8_t[]){255,255,0,255});
    state.color_write_mask=0;
    OK(px_raster_clear(&t,green,5));
    OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,green); CHECK(t.depth[0]==1);
    /* Vertex clamping happens before interpolation, and texture alpha modulates. */
    state=px_raster_default_state();
    for (unsigned i=0;i<3;++i) { p[i].rgba[0]=2; p[i].rgba[1]=-1; p[i].rgba[2]=1; }
    {
        const struct px_raster_image image={1,1,half_red,4};
        const struct px_raster_sampler sampler={PX_RASTER_CLAMP,PX_RASTER_CLAMP,PX_RASTER_NEAREST};
        OK(px_raster_clear(&t,green,5));
        OK(px_raster_draw(&t,p,&image,&sampler,&state));
        pixel(&t,0,0,(const uint8_t[]){255,0,0,64});
    }
    px_raster_free(&t);
}

static void depth_and_comparisons(void)
{
    const int pass[8][3]={{0,0,0},{1,1,1},{1,0,0},{1,1,0},{0,1,0},{1,0,1},{0,1,1},{0,0,1}};
    struct px_raster_target t;
    struct px_raster_vertex p[3];
    struct px_raster_state state=px_raster_default_state();
    OK(px_raster_create(1,1,&t)); triangle(p,0,0);
    for(unsigned comparison=0;comparison<8;++comparison) for(unsigned value=0;value<3;++value) {
        state.depth_compare=(enum px_raster_compare)comparison;
        color(p,red,value);
        OK(px_raster_clear(&t,blue,1));
        OK(px_raster_draw(&t,p,NULL,NULL,&state));
        pixel(&t,0,0,pass[comparison][value]?red:blue);
        CHECK(t.depth[0]==(pass[comparison][value]?value:1));
        state.depth_test=0; state.alpha_compare=(enum px_raster_compare)comparison; state.alpha_reference=1;
        color(p,(const uint8_t[]){255,0,0,(uint8_t)value},0);
        OK(px_raster_clear(&t,blue,1));
        OK(px_raster_draw(&t,p,NULL,NULL,&state));
        pixel(&t,0,0,pass[comparison][value]?(const uint8_t[]){255,0,0,(uint8_t)value}:blue);
        state.depth_test=1; state.alpha_compare=PX_RASTER_ALWAYS;
    }
    state=px_raster_default_state(); state.depth_write=0; color(p,red,-2);
    OK(px_raster_clear(&t,blue,1)); OK(px_raster_draw(&t,p,NULL,NULL,&state));
    pixel(&t,0,0,red); CHECK(t.depth[0]==1);
    color(p,green,2); OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,red);
    state.depth_test=0; state.depth_write=1;
    OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,green); CHECK(t.depth[0]==2);
    px_raster_free(&t);
}

static void winding_and_shared_edges(void)
{
    struct px_raster_target t;
    struct px_raster_vertex p[3], a[3], b[3];
    struct px_raster_state state=px_raster_default_state();
    OK(px_raster_create(4,4,&t)); triangle(p,0,0);
    for(unsigned winding=0;winding<2;++winding) for(unsigned cull=0;cull<4;++cull) {
        state.front=(enum px_raster_winding)winding; state.cull=(enum px_raster_cull)cull;
        OK(px_raster_clear(&t,black,INFINITY));
        OK(px_raster_draw(&t,p,NULL,NULL,&state));
        pixel(&t,0,0,(cull==3 || (cull==1 && winding==0) || (cull==2 && winding==1))?black:white);
    }
    { struct px_raster_vertex temp=p[1];p[1]=p[2];p[2]=temp; }
    state.front=PX_RASTER_CCW; state.cull=PX_RASTER_CULL_BACK;
    OK(px_raster_clear(&t,black,INFINITY)); OK(px_raster_draw(&t,p,NULL,NULL,&state)); pixel(&t,0,0,white);
    a[0]=vertex(0,0,0,0);a[1]=vertex(4,0,0,0);a[2]=vertex(0,4,0,0);
    b[0]=vertex(4,0,0,0);b[1]=vertex(4,4,0,0);b[2]=vertex(0,4,0,0);
    color(a,(const uint8_t[]){255,0,0,128},0);color(b,(const uint8_t[]){255,0,0,128},0);
    state=px_raster_default_state();state.depth_test=0;state.depth_write=0;state.blend=PX_RASTER_SOURCE_ALPHA;
    for(unsigned order=0;order<2;++order) {
        OK(px_raster_clear(&t,black,INFINITY));
        OK(px_raster_draw(&t,order?b:a,NULL,NULL,&state));
        OK(px_raster_draw(&t,order?a:b,NULL,NULL,&state));
        for(unsigned y=0;y<4;++y)for(unsigned x=0;x<4;++x)
            pixel(&t,x,y,(const uint8_t[]){128,0,0,191});
        CHECK(t.candidate_pixels==32);
    }
    /* Additive source alpha: src*srcA + dst per channel, then saturation. */
    state.blend=PX_RASTER_ADD_SOURCE_ALPHA;
    OK(px_raster_clear(&t,(const uint8_t[]){0,40,0,100},INFINITY));
    OK(px_raster_draw(&t,a,NULL,NULL,&state));OK(px_raster_draw(&t,b,NULL,NULL,&state));
    for(unsigned y=0;y<4;++y)for(unsigned x=0;x<4;++x)
        pixel(&t,x,y,(const uint8_t[]){128,40,0,164});
    OK(px_raster_draw(&t,a,NULL,NULL,&state));
    pixel(&t,0,0,(const uint8_t[]){255,40,0,228});pixel(&t,3,3,(const uint8_t[]){128,40,0,164});
    { struct px_raster_state bad=state; bad.blend=(enum px_raster_blend)(PX_RASTER_ADD_SOURCE_ALPHA+1);
      CHECK(px_raster_draw(&t,a,NULL,NULL,&bad)==PX_RASTER_INVALID_ARGUMENT); }
    /* Top horizontal and left vertical edges include exact pixel centers. */
    p[0]=vertex(.5,.5,0,0);p[1]=vertex(3.5,.5,0,0);p[2]=vertex(.5,3.5,0,0);
    state=px_raster_default_state();
    OK(px_raster_clear(&t,black,INFINITY));OK(px_raster_draw(&t,p,NULL,NULL,&state));
    pixel(&t,0,0,white);pixel(&t,2,0,white);pixel(&t,0,2,white);
    pixel(&t,3,0,black);pixel(&t,2,1,black);pixel(&t,0,3,black);
    /* At 128.5 fixed units, ties round away from zero to129: this edge
     * lies just right of the first pixel center, so that pixel is excluded. */
    p[0]=vertex(.501953125,0,0,0);p[1]=vertex(4,0,0,0);p[2]=vertex(.501953125,4,0,0);
    OK(px_raster_clear(&t,black,INFINITY));OK(px_raster_draw(&t,p,NULL,NULL,&state));
    pixel(&t,0,0,black);pixel(&t,1,0,white);
    px_raster_free(&t);
}

static void clipping_and_invalid(void)
{
    struct px_raster_target t;
    struct px_raster_vertex p[3], saved[3];
    struct px_raster_state state=px_raster_default_state(), badstate;
    const struct px_raster_sampler sampler={PX_RASTER_CLAMP,PX_RASTER_CLAMP,PX_RASTER_NEAREST};
    struct px_raster_image badimage={1,1,white,3};
    OK(px_raster_create(4,4,&t));OK(px_raster_clear(&t,black,INFINITY));
    p[0]=vertex(-1000000,-1000000,0,0);p[1]=vertex(1000000,-1000000,0,0);p[2]=vertex(0,1000000,0,0);
    OK(px_raster_draw(&t,p,NULL,NULL,&state));
    for(unsigned y=0;y<4;++y)for(unsigned x=0;x<4;++x)pixel(&t,x,y,white);
    CHECK(t.candidate_pixels==16);
    OK(px_raster_clear(&t,black,INFINITY));triangle(p,0,0);
    memcpy(saved,p,sizeof(p));
    for(unsigned field=0;field<9;++field) {
        double *fields[]={&p[1].x,&p[1].y,&p[1].z,&p[1].u,&p[1].v,&p[1].rgba[0],&p[1].rgba[1],&p[1].rgba[2],&p[1].rgba[3]};
        *fields[field]=field&1?INFINITY:NAN;
        CHECK(px_raster_draw(&t,p,NULL,NULL,&state)==PX_RASTER_INVALID_ARGUMENT);
        CHECK(t.candidate_pixels==0);pixel(&t,0,0,black);memcpy(p,saved,sizeof(p));
    }
    p[0].x=1000000.01;CHECK(px_raster_draw(&t,p,NULL,NULL,&state)==PX_RASTER_LIMIT_EXCEEDED);memcpy(p,saved,sizeof(p));
    p[0].u=1e12+1;CHECK(px_raster_draw(&t,p,NULL,NULL,&state)==PX_RASTER_LIMIT_EXCEEDED);memcpy(p,saved,sizeof(p));
    CHECK(px_raster_draw(&t,p,&badimage,&sampler,&state)==PX_RASTER_INVALID_ARGUMENT);
    badimage.rgba_size=4;CHECK(px_raster_draw(&t,p,&badimage,NULL,&state)==PX_RASTER_INVALID_ARGUMENT);
    badimage.width=UINT32_MAX;CHECK(px_raster_draw(&t,p,&badimage,&sampler,&state)==PX_RASTER_INVALID_ARGUMENT);
    badstate=state;badstate.depth_test=2;CHECK(px_raster_draw(&t,p,NULL,NULL,&badstate)==PX_RASTER_INVALID_ARGUMENT);
    badstate=state;badstate.alpha_compare=(enum px_raster_compare)-1;CHECK(px_raster_draw(&t,p,NULL,NULL,&badstate)==PX_RASTER_INVALID_ARGUMENT);
    badstate=state;badstate.color_write_mask=16;CHECK(px_raster_draw(&t,p,NULL,NULL,&badstate)==PX_RASTER_INVALID_ARGUMENT);
    CHECK(t.candidate_pixels==0);pixel(&t,0,0,black);
    t.candidate_pixels=PX_RASTER_MAX_CANDIDATES-15;
    CHECK(px_raster_draw(&t,p,NULL,NULL,&state)==PX_RASTER_LIMIT_EXCEEDED);
    CHECK(t.candidate_pixels==PX_RASTER_MAX_CANDIDATES-15);pixel(&t,0,0,black);CHECK(t.depth[0]==INFINITY);
    t.candidate_pixels=PX_RASTER_MAX_CANDIDATES-16;
    OK(px_raster_draw(&t,p,NULL,NULL,&state));CHECK(t.candidate_pixels==PX_RASTER_MAX_CANDIDATES);
    OK(px_raster_clear(&t,black,INFINITY));CHECK(t.candidate_pixels==0);
    p[0]=vertex(0,0,0,0);p[1]=vertex(.001,0,0,0);p[2]=vertex(0,.001,0,0);
    OK(px_raster_draw(&t,p,NULL,NULL,&state));CHECK(t.candidate_pixels==0);
    p[0]=vertex(-8,-8,0,0);p[1]=vertex(-4,-8,0,0);p[2]=vertex(-8,-4,0,0);
    OK(px_raster_draw(&t,p,NULL,NULL,&state));CHECK(t.candidate_pixels==0);pixel(&t,0,0,black);
    px_raster_free(&t);
}

struct shader_log { unsigned calls; double last_u, last_v, last_rgba[4]; };
static void probe_shader(void *context, double u, double v, const double rgba[4], double out[4])
{
    struct shader_log *log = context;
    ++log->calls;
    log->last_u = u; log->last_v = v;
    memcpy(log->last_rgba, rgba, sizeof log->last_rgba);
    /* Red from u, green from interpolated blue, out-of-range blue clamps,
     * alpha from v so the alpha test can reject a band. */
    out[0] = u; out[1] = rgba[2]; out[2] = 7; out[3] = v;
}

static void shaded(void)
{
    struct px_raster_target t;
    struct shader_log log = {0};
    struct px_raster_state state = px_raster_default_state();
    OK(px_raster_create(4, 4, &t)); OK(px_raster_clear(&t, black, INFINITY));
    /* u = x/4 and v = y/4 across a 4x4 square drawn as two triangles. */
    struct px_raster_vertex a[3] = {vertex(0, 0, 0, 0), vertex(4, 0, 1, 0), vertex(0, 4, 0, 1)};
    struct px_raster_vertex b[3] = {vertex(4, 0, 1, 0), vertex(4, 4, 1, 1), vertex(0, 4, 0, 1)};
    color(a, (const uint8_t[]){0, 0, 102, 255}, 0); color(b, (const uint8_t[]){0, 0, 102, 255}, 0);
    state.alpha_compare = PX_RASTER_GEQUAL; state.alpha_reference = 64;
    OK(px_raster_draw_shaded(&t, a, probe_shader, &log, &state));
    OK(px_raster_draw_shaded(&t, b, probe_shader, &log, &state));
    CHECK(log.calls == 16);
    /* Row 0 has v = 1/8 -> alpha 32 < 64: rejected; rows 1..3 pass. */
    pixel(&t, 0, 0, black);
    pixel(&t, 1, 1, (const uint8_t[]){96, 102, 255, 96});
    pixel(&t, 3, 3, (const uint8_t[]){223, 102, 255, 223});
    /* Depth fails before shading: no calls for occluded fragments. */
    log.calls = 0;
    color(a, (const uint8_t[]){0, 0, 102, 255}, 1);
    OK(px_raster_draw_shaded(&t, a, probe_shader, &log, &state));
    CHECK(log.calls == 3); /* a covers 3 row-0 pixels, never depth-written (alpha-rejected) */
    CHECK(px_raster_draw_shaded(&t, a, NULL, NULL, &state) == PX_RASTER_INVALID_ARGUMENT);
    /* Exposed sampling matches the documented nearest/repeat convention. */
    const uint8_t texels[16] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160};
    const struct px_raster_image image = {2, 2, texels, sizeof texels};
    const struct px_raster_sampler sampler = {PX_RASTER_REPEAT, PX_RASTER_REPEAT, PX_RASTER_NEAREST};
    double rgba[4];
    CHECK(px_raster_image_valid(&image, &sampler) && !px_raster_image_valid(NULL, &sampler));
    px_raster_sample(&image, &sampler, 1.75, -0.25, rgba);
    CHECK(fabs(rgba[0] - 130 / 255.0) < 1e-12 && fabs(rgba[3] - 160 / 255.0) < 1e-12);
    px_raster_free(&t);
}

struct varying_log { unsigned calls; double at_1_1[4]; };
static void varying_shader(void *context, double u, double v, const double rgba[4],
                           const double varying[4], double out[4])
{
    struct varying_log *log = context;
    (void)rgba;
    ++log->calls;
    if (fabs(u - 1.5 / 4) < 1e-12 && fabs(v - 1.5 / 4) < 1e-12) memcpy(log->at_1_1, varying, sizeof log->at_1_1);
    out[0] = varying[0] / 4; out[1] = -varying[1] / 8; out[2] = varying[2]; out[3] = 1;
}
static void varyings(void)
{
    struct px_raster_target t;
    struct px_raster_state state = px_raster_default_state();
    OK(px_raster_create(4, 4, &t));
    /* Unclamped varyings follow their vertices with the affine weights,
     * independent of the winding swap the rasterizer performs. */
    for (unsigned order = 0; order < 2; ++order) {
        struct varying_log log = {0, {0}};
        OK(px_raster_clear(&t, black, INFINITY));
        struct px_raster_vertex p[3] = {vertex(0, 0, 0, 0), vertex(4, 0, 1, 0), vertex(0, 4, 0, 1)};
        double values[12] = {0, -8, 100, 5,  4, -8, 100, 5,  0, 0, 100, 5};
        if (order) {
            const struct px_raster_vertex keep = p[1]; p[1] = p[2]; p[2] = keep;
            for (unsigned c = 0; c < 4; ++c) {
                const double t1 = values[4 + c]; values[4 + c] = values[8 + c]; values[8 + c] = t1;
            }
        }
        OK(px_raster_draw_varying(&t, p, values, varying_shader, &log, &state));
        CHECK(log.calls == 6); /* x+y<=2: the hypotenuse is not a top-left edge */
        CHECK(log.at_1_1[0] == 1.5 && log.at_1_1[1] == -5 && log.at_1_1[2] == 100 && log.at_1_1[3] == 5);
        pixel(&t, 1, 1, (const uint8_t[]){96, 159, 255, 255});
        pixel(&t, 3, 3, black);
        values[5] = NAN;
        CHECK(px_raster_draw_varying(&t, p, values, varying_shader, &log, &state) == PX_RASTER_INVALID_ARGUMENT);
        values[5] = 1e13;
        CHECK(px_raster_draw_varying(&t, p, values, varying_shader, &log, &state) == PX_RASTER_LIMIT_EXCEEDED);
        CHECK(px_raster_draw_varying(&t, p, NULL, varying_shader, &log, &state) == PX_RASTER_INVALID_ARGUMENT);
        CHECK(px_raster_draw_varying(&t, p, values, NULL, &log, &state) == PX_RASTER_INVALID_ARGUMENT);
    }
    px_raster_free(&t);
}

int main(void)
{
    dimensions();uv_and_interpolation();sampling();alpha_blend();depth_and_comparisons();
    winding_and_shared_edges();clipping_and_invalid();shaded();varyings();
    puts("raster fixed-pixel tests passed");
    return 0;
}
