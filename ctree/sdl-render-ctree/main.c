#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include "ctree.h"

static const int k_node_radius = 20;
static const int k_window_width = 800;
static const int k_window_height = 600;
static const int k_font_size = 15;

static int int_compare(void *a, void *b)
{
    int a_ = *(int *)a;
    int b_ = *(int *)b;

    if (a_ < b_)
    {
        return -1;
    }

    if (a_ == b_)
    {
        return 0;
    }

    return 1;
}

static value_handlers_t g_handlers;
static int k_values[] =
{
    1, 4, 4, 3, 5, 6, 7, 8, 9, 10, 12, 11, 13, 13, 103, 102, 101
};

static tree_t *tree_alloc_and_create()
{
    int *value = &k_values[0];
    tree_t *tree_ = malloc(sizeof(tree_t));
    value_handlers_t *INT = value_handlers_init(&g_handlers, int_compare, NULL, NULL);

    node_t *a = node_create(INT, "a", value++);
    node_t *b = node_create(INT, "b", value++);
    node_t *c = node_create(INT, "c", value++);
    node_t *d = node_create(INT, "d", value++);
    node_t *e = node_create(INT, "e", value++);
    node_t *f = node_create(INT, "f", value++);
    node_t *g = node_create(INT, "g", value++);
    node_t *h = node_create(INT, "h", value++);
    node_t *i = node_create(INT, "i", value++);
    node_t *j = node_create(INT, "j", value++);
    node_t *k = node_create(INT, "k", value++);
    node_t *l = node_create(INT, "l", value++);
    node_t *m = node_create(INT, "m", value++);
    node_t *n = node_create(INT, "n", value++);
    node_t *o = node_create(INT, "o", value++);
    node_t *p = node_create(INT, "p", value++);
    node_t *r = node_create(INT, "r", value++);

    tree_init(tree_);
    tree_add_node(tree_, a, NULL);
    tree_add_node(tree_, b, "a");
    tree_add_node(tree_, c, "a");
    tree_add_node(tree_, d, "a");
    tree_add_node(tree_, e, "c");
    tree_add_node(tree_, f, "c");
    tree_add_node(tree_, g, "f");
    tree_add_node(tree_, h, "f");
    tree_add_node(tree_, i, "d");
    tree_add_node(tree_, j, "i");
    tree_add_node(tree_, k, "j");
    tree_add_node(tree_, l, "j");
    tree_add_node(tree_, m, "j");
    tree_add_node(tree_, n, "j");
    tree_add_node(tree_, o, "b");
    tree_add_node(tree_, p, "b");
    tree_add_node(tree_, r, "b");
    return tree_;
}

static void tree_dispose_and_free(tree_t *t)
{
    tree_clear(t);
    free(t);
}

static char k_buffer[16];
static const SDL_Color k_font_color =
{
    0, 0, 0, SDL_ALPHA_OPAQUE
};

static void render_node(SDL_Renderer *renderer, TTF_Font *font, int x_pos, int y_pos, node_t *node)
{
    int text_width = 0;
    int text_height = 0;

    filledCircleRGBA(renderer, x_pos, y_pos, k_node_radius, 0, 255, 0, SDL_ALPHA_OPAQUE);
    circleRGBA(renderer, x_pos, y_pos, k_node_radius, 0, 0, 0, SDL_ALPHA_OPAQUE);

    snprintf(k_buffer, 16, "%s:%d", node->object_name, *((int*)node->value));

    SDL_Surface * surface = TTF_RenderText_Solid(font, k_buffer, k_font_color);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);

    SDL_Rect rect =
    {
        x_pos - text_width / 2, y_pos - 2 * k_node_radius, text_width, text_height
    };

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

static void render_tree_step(
    SDL_Renderer *renderer,
    TTF_Font *font,
    tree_t *tree,
    int y_pos,
    int y_delta,
    int x_start,
    int x_end,
    node_t *node)
{
    if (node->num_children == 0)
    {
        return;
    }

    int x_delta = (x_end - x_start) / node->num_children;

    for (int i = 0; i < node->num_children; ++i)
    {
        render_tree_step(
            renderer,
            font,
            tree,
            y_pos + y_delta,
            y_delta,
            x_start + i * x_delta,
            x_start + (i + 1) * x_delta,
            node->children[i]);

        SDL_RenderDrawLine(
            renderer,
            (x_start + x_end) / 2, /* parent's x */
            y_pos, /* parent's y */
            x_start + (2 * i + 1) * x_delta / 2, /* child's x */
            y_pos + y_delta); /* child's y */

        render_node(
            renderer,
            font,
            x_start + (2 * i + 1) * x_delta / 2,
            y_pos + y_delta,
            node->children[i]);
    }
}

void render_tree(SDL_Renderer *renderer, TTF_Font *font, tree_t *tree)
{
    int y_delta = (k_window_height - 6 * k_node_radius) / (tree_depth(tree)- 1);

    int root_y = k_node_radius * 3;

    render_tree_step(
        renderer,
        font,
        tree,
        root_y,
        y_delta,
        k_node_radius * 2,
        k_window_width - 2 * k_node_radius,
        tree->root);

    render_node(renderer, font, k_window_width / 2, root_y, tree->root);
}

static const char *k_font_file = "DejaVuSansMono.ttf";

int main()
{
    TTF_Font *font = NULL;
    TTF_Init();
    font = TTF_OpenFont(k_font_file, k_font_size);

    if (font == NULL)
    {
        printf("Cannot find font in working directory: %s!\n", k_font_file);
        TTF_Quit();
        return -1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        tree_t *tree = tree_alloc_and_create();
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;

        if ((font != NULL) &&
            (SDL_CreateWindowAndRenderer(k_window_width, k_window_height, 0, &window, &renderer) == 0))
        {
            SDL_bool done = SDL_FALSE;

            while (done == SDL_FALSE)
            {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

                render_tree(renderer, font, tree);

                SDL_RenderPresent(renderer);

                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        done = SDL_TRUE;
                    }
                }
            }
        }

        if (renderer != NULL)
        {
            SDL_DestroyRenderer(renderer);
        }

        if (window != NULL)
        {
            SDL_DestroyWindow(window);
        }

        tree_dispose_and_free(tree);

        TTF_CloseFont(font);
    }

    SDL_Quit();
    TTF_Quit();
    return 0;
}

