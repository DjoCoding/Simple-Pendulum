#include <stdio.h>
#include <raylib.h>

#define VEC2_IMPLEMENTATION
#include "vec2.h"

#include "da.h"

#define g 10

#define WINDOW_WIDTH  1366
#define WINDOW_HEIGHT 768

typedef struct {
    fV2   c;        // the current ball center
    float angle0;   // the inital angle
    float v0;       // the initial velocity
    float a;        // the angle acceleration
    float m;        // mass
    float r;        // radius
    float angle;    // angle
} Ball;

typedef struct {
    fV2 fp;            // fixed point
    float l;           // length
    Ball b;            // the actual ball
    float t;           // the current time
} Pendulum;

Ball ball_init(float a, float m, float r, float angle, float v0) {
    Ball b = {0};
    b.a = a;
    b.m = m;
    b.r = r;
    b.angle0 = angle;
    b.angle = angle;
    b.v0 = v0;
    return b;
}

Pendulum pendulum_init(fV2 fp, float l, Ball b) {
    Pendulum p = {0};
    p.fp = fp;
    p.l = l;
    p.b = b;
    return p;
}

void pendulum_update_acc(Pendulum *p) {
    p->b.a = -g * sin(p->b.angle) / p->l;
}

void pendulum_update_angle(Pendulum *p) {
    float k = (float) g / p->l;
    p->b.angle = p->b.angle0 * cos(sqrt(k) * p->t) + (p->b.v0 / sqrt(k)) * sin(sqrt(k) * p->t); 
}

void pendulum_set_ball_center(Pendulum *p) {
    p->b.c = fv2_add(p->fp, fv2(p->l * sin(p->b.angle), p->l * cos(p->b.angle)));
}

void pendulum_update_time(Pendulum *p, float dt) {
    p->t += dt;
}

void pendulum_render(Pendulum *p) {
    DrawLine(p->fp.x - 10, p->fp.y, p->fp.x + 10, p->fp.y, RAYWHITE);
    DrawLine(p->fp.x, p->fp.y, p->b.c.x, p->b.c.y, RAYWHITE);
    DrawCircle(p->b.c.x, p->b.c.y, p->b.r, RED);
}

#define MASS   10
#define RADIUS 20.0f
#define L      300

void draw_middle_line() {
    DrawLine(WINDOW_WIDTH / 2, 0, WINDOW_WIDTH / 2, WINDOW_HEIGHT, RAYWHITE);
}

#define UNIT_SCALE 30
#define PADDING    10

typedef struct {
    fV2 *items;
    size_t count;
    size_t size;
} Tracer;

typedef struct {
    fV2 origin;
    fV2 unit;
    fV2 ymax;
    fV2 xmax;
    fV2 xboundaries;
    fV2 yboundaries;
    Tracer tracer;
    float t; 
} Plot;

Plot plot_init(fV2 unit, fV2 xmax, fV2 ymax) {
    Plot p = {0};

    p.origin = fv2(unit.x * 2 * UNIT_SCALE * (-xmax.x + 2), WINDOW_HEIGHT + (unit.y * UNIT_SCALE * (ymax.x - 2)) - PADDING);
    p.unit = unit;
    p.xmax = xmax;
    p.ymax = ymax;
    
    printf("%f, %f\n", p.origin.x, p.origin.y);
    return p;
}

void plot_append_point(Plot *p, float dt, float y) {
    p->t += dt;
    DA_APPEND(&p->tracer, fv2(p->t, y));
}

void plot_set_axis_boundaries(Plot *p) {
    float ybottom = p->origin.y - p->ymax.x * p->unit.y * UNIT_SCALE;
    float ytop    = p->origin.y - p->ymax.y * p->unit.y * UNIT_SCALE;
    
    if (ytop < WINDOW_HEIGHT / 2) {
        ytop = WINDOW_HEIGHT / 2;
    }

    float xleft = p->origin.x + p->xmax.x * p->unit.x * UNIT_SCALE; 
    float xright = p->origin.x + p->xmax.y * p->unit.x * UNIT_SCALE;    

    p->xboundaries = fv2(xleft, xright);
    p->yboundaries = fv2(ybottom, ytop);
}

void plot_axis_render(Plot *p) {
    // this is for the vertical axis (y axis)
    DrawLine(p->origin.x, p->yboundaries.x + PADDING, p->origin.x, p->yboundaries.y - PADDING, RAYWHITE);

    // the y axis arrow
    DrawLine(p->origin.x - PADDING, p->yboundaries.y, p->origin.x, p->yboundaries.y - PADDING, RAYWHITE);
    DrawLine(p->origin.x + PADDING, p->yboundaries.y, p->origin.x, p->yboundaries.y - PADDING, RAYWHITE);


    // this is for the horizontal axis (x axis)
    DrawLine(p->xboundaries.x - PADDING, p->origin.y, p->xboundaries.y + PADDING, p->origin.y, RAYWHITE);

    // the x axis arrow
    DrawLine(p->xboundaries.y - PADDING, p->origin.y + PADDING, p->xboundaries.y + PADDING, p->origin.y, RAYWHITE);
    DrawLine(p->xboundaries.y - PADDING, p->origin.y - PADDING, p->xboundaries.y + PADDING, p->origin.y, RAYWHITE); 
}

void plot_units_render(Plot *p) {    
    for(float x = p->xboundaries.x; x < p->xboundaries.y; x += p->unit.x * UNIT_SCALE) {
        DrawLine(x, p->origin.y - 10, x, p->origin.y + 10, RAYWHITE);
    } 

    for(float y = p->yboundaries.x; y > p->yboundaries.y; y -= p->unit.y * UNIT_SCALE) {
        DrawLine(p->origin.x - 10, y, p->origin.x + 10, y, RAYWHITE);
    }
}

void plot_render_curve(Plot *p) {
    if (p->tracer.count <= 1) { return; }

    for (size_t i = 0; i < p->tracer.count - 1; ++i) {
        fV2 current_local = p->tracer.items[i];
        if (current_local.x > ((p->xboundaries.y - PADDING) / ( UNIT_SCALE * p->unit.x))) { return; }

        fV2 next_local = p->tracer.items[i + 1];

        current_local.x *= UNIT_SCALE * p->unit.x;
        current_local.y *= UNIT_SCALE * p->unit.y;

        fV2 current_world = fv2_add(p->origin, current_local);

        next_local.x *= UNIT_SCALE * p->unit.x;
        next_local.y *= UNIT_SCALE * p->unit.y;
        fV2 next_world = fv2_add(p->origin, next_local);


        DrawLine(current_world.x, current_world.y, next_world.x, next_world.y, RED);
    }
}

void plot_render(Plot *p) {
    plot_axis_render(p);
    plot_units_render(p);
    plot_render_curve(p);
}

void plot_clean(Plot *p) {
    free(p->tracer.items);
}

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Pendulum");

    fV2 fp = fv2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 10);
    Ball b = ball_init(0, MASS, RADIUS, PI / 2, 0);
    Pendulum p = pendulum_init(fp, 2 * L / 5, b);
    Plot plot = plot_init(fv2(0.5, 1), fv2(-1, 80), fv2(-4, 4));
    plot_set_axis_boundaries(&plot);

    SetTargetFPS(60);
    float dt = 0.1f;

    while(!WindowShouldClose()) {


        if (!IsKeyDown(KEY_SPACE)) {
            pendulum_update_acc(&p);
            pendulum_update_time(&p, dt);
            pendulum_update_angle(&p);
            pendulum_set_ball_center(&p);
            plot_append_point(&plot, dt, p.b.angle);
        }
        

        BeginDrawing();
        {
            ClearBackground(BLACK);
            pendulum_render(&p);
            plot_render(&plot);
            // printf("%f\n", plot.t);
            // printf("%f\n", p.b.angle * 180 / PI);
        }
        EndDrawing();
    }
    
    plot_clean(&plot);
    CloseWindow();

    return 0;
}

