#include <math.h>
#include <string.h>
#include "../libkc3/kc3.h"
#include "test.h"

#if HAVE_GLESV2

#include "../gl/gles2.h"
#include "../gl/types.h"

static uw g_mesh_buffer_calls;
static uw g_mesh_buffers;
static uw g_mesh_fail_buffer;
static uw g_mesh_init_calls;
static uw g_mesh_fail_init;
static uw g_mesh_bad_size;
static uw g_mesh_alloc_calls;
static uw g_mesh_fail_alloc;
static uw g_mesh_arrays;
static uw g_mesh_dimensions;
static uw g_mesh_uploads;
static bool g_mesh_error;

TEST_CASE(gl_mesh_types)
{
  const s_sym *vertex = sym_1("GL.Vertex");
  const s_sym *triangle = sym_1("GL.Triangle");
  TEST_ASSERT(module_load(vertex));
  TEST_ASSERT(module_load(triangle));
}
TEST_CASE_END(gl_mesh_types)

static void mesh_reset (void)
{
  g_mesh_buffer_calls = 0;
  g_mesh_buffers = 0;
  g_mesh_fail_buffer = 0;
  g_mesh_init_calls = 0;
  g_mesh_fail_init = 0;
  g_mesh_bad_size = 0;
  g_mesh_alloc_calls = 0;
  g_mesh_fail_alloc = 0;
  g_mesh_arrays = 0;
  g_mesh_dimensions = 0;
  g_mesh_uploads = 0;
  g_mesh_error = false;
}

static void mesh_gen_buffers (GLsizei n, GLuint *buffers)
{
  while (n-- > 0) {
    g_mesh_buffer_calls++;
    if (g_mesh_buffer_calls == g_mesh_fail_buffer)
      *buffers++ = 0;
    else {
      *buffers++ = g_mesh_buffer_calls;
      g_mesh_buffers++;
    }
  }
}

static void mesh_delete_buffers (GLsizei n, const GLuint *buffers)
{
  while (n-- > 0) {
    if (! *buffers++ || ! g_mesh_buffers)
      g_mesh_error = true;
    else
      g_mesh_buffers--;
  }
}

static GLenum mesh_get_error (void)
{
  return GL_NO_ERROR;
}

static void mesh_bind_buffer (GLenum target, GLuint buffer)
{
  (void) target;
  (void) buffer;
}

static void mesh_buffer_data (GLenum target, GLsizeiptr size,
                              const void *data, GLenum usage)
{
  (void) target;
  (void) usage;
  if (size <= 0 || ! data)
    g_mesh_error = true;
  g_mesh_uploads++;
}

static void mesh_vertex_attrib_pointer (GLuint index, GLint size,
                                       GLenum type, GLboolean normalized,
                                       GLsizei stride, const void *data)
{
  (void) index;
  (void) size;
  (void) type;
  (void) normalized;
  (void) stride;
  (void) data;
}

static void mesh_enable_vertex_attrib (GLuint index)
{
  (void) index;
}

static void mesh_draw_elements (GLenum mode, GLsizei count, GLenum type,
                                const void *indices)
{
  (void) mode;
  (void) count;
  (void) type;
  (void) indices;
}

static void mesh_draw_arrays (GLenum mode, GLint first, GLsizei count)
{
  (void) mode;
  (void) first;
  (void) count;
}

static void mesh_vertex_attrib (void)
{
}

static void mesh_vertex_transform (s_gl_vertex *vertex,
                                   const s_mat4 *matrix)
{
  (void) vertex;
  (void) matrix;
}

static const char * mesh_error_string (s32 error)
{
  (void) error;
  return "mock GL error";
}

static s_array * mesh_array_init (s_array *array, const s_sym *type,
                                  uw dimensions, const uw *counts)
{
  g_mesh_init_calls++;
  if (g_mesh_init_calls == g_mesh_fail_init ||
      ! array_init(array, type, dimensions, counts))
    return NULL;
  if (array->dimensions)
    g_mesh_dimensions++;
  if (g_mesh_init_calls == g_mesh_bad_size)
    array->dimensions[0].item_size++;
  return array;
}

static s_array * mesh_array_allocate (s_array *array)
{
  u8 *data;
  g_mesh_alloc_calls++;
  if (g_mesh_alloc_calls == g_mesh_fail_alloc ||
      ! array->size || array->size > 1024 * 1024)
    return NULL;
  if (! (data = alloc(array->size + 64)))
    return NULL;
  memset(data, 0xA5, array->size + 64);
  memset(data + 32, 0xFF, array->size);
  array->free_data = data;
  array->data = data + 32;
  g_mesh_arrays++;
  return array;
}

static void mesh_array_clean (s_array *array)
{
  uw i;
  u8 *data = array->free_data;
  if (data) {
    for (i = 0; i < 32; i++)
      if (data[i] != 0xA5 || data[32 + array->size + i] != 0xA5)
        g_mesh_error = true;
    if (! g_mesh_arrays)
      g_mesh_error = true;
    else
      g_mesh_arrays--;
  }
  if (array->dimensions) {
    if (! g_mesh_dimensions)
      g_mesh_error = true;
    else
      g_mesh_dimensions--;
  }
  array_clean(array);
}

#define glGenBuffers mesh_gen_buffers
#define glDeleteBuffers mesh_delete_buffers
#define glGetError mesh_get_error
#define glBindBuffer mesh_bind_buffer
#define glBufferData mesh_buffer_data
#define glVertexAttribPointer mesh_vertex_attrib_pointer
#define glEnableVertexAttribArray mesh_enable_vertex_attrib
#define glDrawElements mesh_draw_elements
#define glDrawArrays mesh_draw_arrays
#define gl_vertex_attrib mesh_vertex_attrib
#define gl_vertex_transform mesh_vertex_transform
#define gl_error_string mesh_error_string
#define array_init mesh_array_init
#define array_allocate mesh_array_allocate
#define array_clean mesh_array_clean
#define gl_object_allocate mesh_object_allocate
#define gl_object_clean mesh_object_clean
#define gl_object_init mesh_object_init
#define gl_object_render mesh_object_render
#define gl_object_render_points mesh_object_render_points
#define gl_object_render_triangles mesh_object_render_triangles
#define gl_object_render_wireframe mesh_object_render_wireframe
#define gl_object_size_valid mesh_object_size_valid
#define gl_object_transform mesh_object_transform
#define gl_object_update mesh_object_update
#define gl_sphere_clean mesh_sphere_clean
#define gl_sphere_delete mesh_sphere_delete
#define gl_sphere_init mesh_sphere_init
#define gl_sphere_new mesh_sphere_new
#define gl_sphere_render mesh_sphere_render
#define gl_square_clean mesh_square_clean
#define gl_square_delete mesh_square_delete
#define gl_square_init mesh_square_init
#define gl_square_new mesh_square_new
#define gl_square_render mesh_square_render
#define gl_square_render_wireframe mesh_square_render_wireframe
#define gl_box_clean mesh_box_clean
#define gl_box_delete mesh_box_delete
#define gl_box_init mesh_box_init
#define gl_box_new mesh_box_new
#define gl_box_render mesh_box_render
#define gl_box_render_points mesh_box_render_points
#define gl_box_render_triangles mesh_box_render_triangles
#define gl_box_render_wireframe mesh_box_render_wireframe
#define gl_cylinder_clean mesh_cylinder_clean
#define gl_cylinder_init mesh_cylinder_init
#define gl_cylinder_render mesh_cylinder_render

#include "../gl/gl_object.c"
#include "../gl/gl_sphere.c"
#include "../gl/gl_square.c"
#include "../gl/gl_box.c"
#include "../gl/gl_cylinder.c"

static char mesh_check (const s_gl_object *object, uw vertices,
                        uw triangles)
{
  uw i;
  const s_gl_triangle *triangle = object->triangle.data;
  const s_gl_vertex *vertex = object->vertex.data;
  TEST_EQ(object->vertex.count, vertices);
  TEST_EQ(object->triangle.count, triangles);
  TEST_EQ(object->vertex.size, vertices * sizeof(*vertex));
  TEST_EQ(object->triangle.size, triangles * sizeof(*triangle));
  for (i = 0; i < vertices; i++) {
    TEST_ASSERT(isfinite(vertex[i].pos_x));
    TEST_ASSERT(isfinite(vertex[i].pos_y));
    TEST_ASSERT(isfinite(vertex[i].pos_z));
  }
  for (i = 0; i < triangles; i++) {
    TEST_ASSERT(triangle[i].a < vertices);
    TEST_ASSERT(triangle[i].b < vertices);
    TEST_ASSERT(triangle[i].c < vertices);
    TEST_ASSERT(triangle[i].a != triangle[i].b);
    TEST_ASSERT(triangle[i].b != triangle[i].c);
    TEST_ASSERT(triangle[i].a != triangle[i].c);
  }
  return 0;
}

static char mesh_check_clean (void)
{
  TEST_EQ(g_mesh_buffers, 0);
  TEST_EQ(g_mesh_arrays, 0);
  TEST_EQ(g_mesh_dimensions, 0);
  TEST_EQ(g_mesh_error, false);
  return 0;
}

TEST_CASE(gl_mesh_sphere_reject)
{
  const u32 dimensions[][2] = {
    {65536, 65536}, {U32_MAX, 1}, {3, U32_MAX},
    {U32_MAX, U32_MAX}, {3, U32_MAX - 1}, {32768, 32768}
  };
  uw i;
  s_gl_sphere saved = {.segments_u = 17};
  s_gl_sphere sphere;
  for (i = 0; i < sizeof(dimensions) / sizeof(dimensions[0]); i++) {
    mesh_reset();
    g_mesh_fail_buffer = 1;
    sphere = saved;
    TEST_EQ(gl_sphere_init(&sphere, dimensions[i][0], dimensions[i][1]),
            NULL);
    TEST_EQ(g_mesh_buffer_calls, 0);
    TEST_EQ(memcmp(&sphere, &saved, sizeof(saved)), 0);
    TEST_EQ(mesh_check_clean(), 0);
  }
}
TEST_CASE_END(gl_mesh_sphere_reject)

TEST_CASE(gl_mesh_square_reject)
{
  const u32 dimensions[][2] = {
    {65536, 65536}, {U32_MAX, 2}, {2, U32_MAX},
    {U32_MAX, U32_MAX}, {32768, 32768}
  };
  uw i;
  s_gl_square saved = {.segments_u = 17};
  s_gl_square square;
  for (i = 0; i < sizeof(dimensions) / sizeof(dimensions[0]); i++) {
    mesh_reset();
    g_mesh_fail_buffer = 1;
    square = saved;
    TEST_EQ(gl_square_init(&square, dimensions[i][0], dimensions[i][1]),
            NULL);
    TEST_EQ(g_mesh_buffer_calls, 0);
    TEST_EQ(memcmp(&square, &saved, sizeof(saved)), 0);
    TEST_EQ(mesh_check_clean(), 0);
  }
}
TEST_CASE_END(gl_mesh_square_reject)

TEST_CASE(gl_mesh_box_reject)
{
  const u32 dimensions[][3] = {
    {65536, 65536, 65536}, {U32_MAX, 1, 1}, {1, U32_MAX, 1},
    {1, 1, U32_MAX}, {U32_MAX, U32_MAX, U32_MAX},
    {20000, 20000, 20000}
  };
  uw i;
  s_gl_box saved = {.segments_u = 17};
  s_gl_box box;
  for (i = 0; i < sizeof(dimensions) / sizeof(dimensions[0]); i++) {
    mesh_reset();
    g_mesh_fail_buffer = 1;
    box = saved;
    TEST_EQ(gl_box_init(&box, dimensions[i][0], dimensions[i][1],
                        dimensions[i][2]), NULL);
    TEST_EQ(g_mesh_buffer_calls, 0);
    TEST_EQ(memcmp(&box, &saved, sizeof(saved)), 0);
    TEST_EQ(mesh_check_clean(), 0);
  }
}
TEST_CASE_END(gl_mesh_box_reject)

TEST_CASE(gl_mesh_cylinder_reject)
{
  const u32 dimensions[][2] = {
    {65536, 65536}, {U32_MAX, 1}, {1, U32_MAX},
    {U32_MAX, U32_MAX}, {0, 1}, {1, 0}
  };
  uw i;
  s_gl_cylinder saved = {.segments_u = 17};
  s_gl_cylinder cylinder;
  for (i = 0; i < sizeof(dimensions) / sizeof(dimensions[0]); i++) {
    mesh_reset();
    g_mesh_fail_buffer = 1;
    cylinder = saved;
    TEST_EQ(gl_cylinder_init(&cylinder, dimensions[i][0],
                             dimensions[i][1]), NULL);
    TEST_EQ(g_mesh_buffer_calls, 0);
    TEST_EQ(memcmp(&cylinder, &saved, sizeof(saved)), 0);
    TEST_EQ(mesh_check_clean(), 0);
  }
}
TEST_CASE_END(gl_mesh_cylinder_reject)

TEST_CASE(gl_mesh_sphere_small)
{
  u32 u;
  u32 v;
  s_gl_sphere sphere;
  for (u = 0; u < 7; u++)
    for (v = 0; v < 5; v++) {
      mesh_reset();
      TEST_EQ(gl_sphere_init(&sphere, u, v), &sphere);
      TEST_EQ(sphere.segments_u, u < 3 ? 3 : u);
      TEST_EQ(sphere.segments_v, v < 1 ? 1 : v);
      TEST_EQ(mesh_check(&sphere.object,
                         (sphere.segments_u + 1) *
                         (sphere.segments_v + 2),
                         2 * sphere.segments_u *
                         (sphere.segments_v + 1)), 0);
      gl_sphere_clean(&sphere);
      TEST_EQ(mesh_check_clean(), 0);
    }
}
TEST_CASE_END(gl_mesh_sphere_small)

TEST_CASE(gl_mesh_square_small)
{
  u32 u;
  u32 v;
  s_gl_square square;
  for (u = 0; u < 6; u++)
    for (v = 0; v < 6; v++) {
      mesh_reset();
      TEST_EQ(gl_square_init(&square, u, v), &square);
      TEST_EQ(square.segments_u, u < 2 ? 2 : u);
      TEST_EQ(square.segments_v, v < 2 ? 2 : v);
      TEST_EQ(mesh_check(&square.object,
                         square.segments_u * square.segments_v,
                         2 * (square.segments_u - 1) *
                         (square.segments_v - 1)), 0);
      TEST_EQ(g_mesh_uploads, 2);
      gl_square_clean(&square);
      TEST_EQ(mesh_check_clean(), 0);
    }
}
TEST_CASE_END(gl_mesh_square_small)

TEST_CASE(gl_mesh_box_small)
{
  u32 u;
  u32 v;
  u32 w;
  u32 a;
  u32 b;
  u32 c;
  s_gl_box box;
  for (u = 0; u < 4; u++)
    for (v = 0; v < 4; v++)
      for (w = 0; w < 4; w++) {
        mesh_reset();
        a = u ? u : 1;
        b = v ? v : 1;
        c = w ? w : 1;
        TEST_EQ(gl_box_init(&box, u, v, w), &box);
        TEST_EQ(box.segments_u, a);
        TEST_EQ(box.segments_v, b);
        TEST_EQ(box.segments_w, c);
        TEST_EQ(mesh_check(&box.object,
                           2 * ((a + 1) * (b + 1) + (b + 1) * (c + 1) +
                                (c + 1) * (a + 1)),
                           4 * (a * b + b * c + c * a)), 0);
        gl_box_clean(&box);
        TEST_EQ(mesh_check_clean(), 0);
      }
}
TEST_CASE_END(gl_mesh_box_small)

TEST_CASE(gl_mesh_cylinder_small)
{
  u32 u;
  u32 v;
  s_gl_cylinder cylinder;
  for (u = 1; u < 6; u++)
    for (v = 1; v < 6; v++) {
      mesh_reset();
      TEST_EQ(gl_cylinder_init(&cylinder, u, v), &cylinder);
      TEST_EQ(cylinder.object.vertex.count, u * v + 2);
      TEST_EQ(cylinder.object.triangle.count, 6 * (u + 1) * (v + 2));
      gl_cylinder_clean(&cylinder);
      TEST_EQ(mesh_check_clean(), 0);
    }
}
TEST_CASE_END(gl_mesh_cylinder_small)

TEST_CASE(gl_mesh_failures)
{
  uw fail;
  uw shape;
  s_gl_box box = {0};
  s_gl_cylinder cylinder = {0};
  s_gl_sphere sphere = {0};
  s_gl_square square = {0};
  for (shape = 0; shape < 4; shape++)
    for (fail = 0; fail < 8; fail++) {
      mesh_reset();
      if (fail < 2)
        g_mesh_fail_buffer = fail + 1;
      else if (fail < 4)
        g_mesh_fail_init = fail - 1;
      else if (fail < 6)
        g_mesh_fail_alloc = fail - 3;
      else
        g_mesh_bad_size = fail - 5;
      switch (shape) {
      case 0:
        TEST_EQ(gl_sphere_init(&sphere, 5, 4), NULL);
        TEST_EQ(sphere.segments_u, 0);
        TEST_EQ(sphere.object.gl_vbo, 0);
        break;
      case 1:
        TEST_EQ(gl_square_init(&square, 5, 4), NULL);
        TEST_EQ(square.segments_u, 0);
        TEST_EQ(square.object.gl_vbo, 0);
        break;
      case 2:
        TEST_EQ(gl_box_init(&box, 5, 4, 3), NULL);
        TEST_EQ(box.segments_u, 0);
        TEST_EQ(box.object.gl_vbo, 0);
        break;
      case 3:
        TEST_EQ(gl_cylinder_init(&cylinder, 5, 4), NULL);
        TEST_EQ(cylinder.segments_u, 0);
        TEST_EQ(cylinder.object.gl_vbo, 0);
        break;
      }
      TEST_EQ(mesh_check_clean(), 0);
    }
}
TEST_CASE_END(gl_mesh_failures)

TEST_CASE(gl_mesh_allocate_reject)
{
  const uw counts[][2] = {
    {UW_MAX, 1}, {1, UW_MAX}, {0, 1}, {1, 0},
    {SW_MAX / sizeof(s_gl_vertex) + 1, 1},
    {1, SW_MAX / sizeof(s_gl_triangle) + 1},
    {1, S32_MAX / 3 + 1}
  };
  uw i;
  s_gl_object object = {0};
  for (i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
    mesh_reset();
    g_mesh_fail_alloc = 1;
    TEST_EQ(gl_object_allocate(&object, counts[i][0], counts[i][1]),
            NULL);
    TEST_EQ(g_mesh_init_calls, 0);
    TEST_EQ(mesh_check_clean(), 0);
  }
}
TEST_CASE_END(gl_mesh_allocate_reject)

TEST_CASE(gl_mesh_allocate_replace)
{
  s_gl_object object = {0};
  s_gl_object saved;
  mesh_reset();
  TEST_EQ(gl_object_allocate(&object, 4, 2), &object);
  saved = object;
  g_mesh_fail_alloc = g_mesh_alloc_calls + 2;
  TEST_EQ(gl_object_allocate(&object, 9, 8), NULL);
  TEST_EQ(memcmp(&object, &saved, sizeof(saved)), 0);
  TEST_EQ(g_mesh_arrays, 2);
  TEST_EQ(g_mesh_dimensions, 2);
  g_mesh_fail_alloc = 0;
  TEST_EQ(gl_object_allocate(&object, 9, 8), &object);
  TEST_EQ(object.vertex.count, 9);
  TEST_EQ(object.triangle.count, 8);
  TEST_EQ(g_mesh_arrays, 2);
  TEST_EQ(g_mesh_dimensions, 2);
  gl_object_clean(&object);
  TEST_EQ(mesh_check_clean(), 0);
}
TEST_CASE_END(gl_mesh_allocate_replace)

TEST_CASE(gl_mesh_size_limits)
{
  u64 vertices = (u64) SW_MAX / sizeof(s_gl_vertex);
  u64 triangles = (u64) SW_MAX / sizeof(s_gl_triangle);
  if (vertices > S32_MAX)
    vertices = S32_MAX;
  if (triangles > S32_MAX / 3)
    triangles = S32_MAX / 3;
  TEST_ASSERT(gl_object_size_valid(1, 1));
  TEST_ASSERT(gl_object_size_valid(vertices, triangles));
  TEST_ASSERT(! gl_object_size_valid(vertices + 1, triangles));
  TEST_ASSERT(! gl_object_size_valid(vertices, triangles + 1));
  TEST_ASSERT(! gl_object_size_valid(0, 1));
  TEST_ASSERT(! gl_object_size_valid(1, 0));
  TEST_ASSERT(! gl_object_size_valid(U64_MAX, 1));
  TEST_ASSERT(! gl_object_size_valid(1, U64_MAX));
}
TEST_CASE_END(gl_mesh_size_limits)

TEST_CASE(gl_mesh_dimension_limits)
{
  uw axis;
  uw step;
  u32 limit;
  u32 dimension;
  u64 vertices = (u64) SW_MAX / sizeof(s_gl_vertex);
  u64 triangles = (u64) SW_MAX / sizeof(s_gl_triangle);
  s_gl_box box = {0};
  s_gl_cylinder cylinder = {0};
  s_gl_sphere sphere = {0};
  s_gl_square square = {0};
  if (vertices > S32_MAX)
    vertices = S32_MAX;
  if (triangles > S32_MAX / 3)
    triangles = S32_MAX / 3;
  limit = vertices / 4 - 2;
  if (limit > triangles / 6 - 1)
    limit = triangles / 6 - 1;
  for (step = 0; step < 2; step++) {
    mesh_reset();
    g_mesh_fail_alloc = 1;
    TEST_EQ(gl_sphere_init(&sphere, 3, limit + step), NULL);
    TEST_EQ(g_mesh_buffer_calls, step ? 0 : 2);
    TEST_EQ(g_mesh_init_calls, step ? 0 : 1);
    TEST_EQ(mesh_check_clean(), 0);
  }
  limit = vertices / 3 - 1;
  if (limit > triangles / 4)
    limit = triangles / 4;
  for (step = 0; step < 2; step++) {
    mesh_reset();
    g_mesh_fail_alloc = 1;
    TEST_EQ(gl_sphere_init(&sphere, limit + step, 1), NULL);
    TEST_EQ(g_mesh_buffer_calls, step ? 0 : 2);
    TEST_EQ(g_mesh_init_calls, step ? 0 : 1);
    TEST_EQ(mesh_check_clean(), 0);
  }
  limit = vertices / 2;
  if (limit > triangles / 2 + 1)
    limit = triangles / 2 + 1;
  for (axis = 0; axis < 2; axis++)
    for (step = 0; step < 2; step++) {
      mesh_reset();
      g_mesh_fail_alloc = 1;
      dimension = limit + step;
      TEST_EQ(gl_square_init(&square, axis ? 2 : dimension,
                             axis ? dimension : 2), NULL);
      TEST_EQ(g_mesh_buffer_calls, step ? 0 : 2);
      TEST_EQ(g_mesh_init_calls, step ? 0 : 1);
      TEST_EQ(mesh_check_clean(), 0);
    }
  limit = (vertices - 16) / 8;
  if (limit > (triangles - 4) / 8)
    limit = (triangles - 4) / 8;
  for (axis = 0; axis < 3; axis++)
    for (step = 0; step < 2; step++) {
      mesh_reset();
      g_mesh_fail_alloc = 1;
      dimension = limit + step;
      TEST_EQ(gl_box_init(&box, axis == 0 ? dimension : 1,
                          axis == 1 ? dimension : 1,
                          axis == 2 ? dimension : 1), NULL);
      TEST_EQ(g_mesh_buffer_calls, step ? 0 : 2);
      TEST_EQ(g_mesh_init_calls, step ? 0 : 1);
      TEST_EQ(mesh_check_clean(), 0);
    }
  limit = vertices - 2;
  if (limit > triangles / 12 - 2)
    limit = triangles / 12 - 2;
  for (step = 0; step < 2; step++) {
    mesh_reset();
    g_mesh_fail_alloc = 1;
    TEST_EQ(gl_cylinder_init(&cylinder, 1, limit + step), NULL);
    TEST_EQ(g_mesh_buffer_calls, step ? 0 : 2);
    TEST_EQ(g_mesh_init_calls, step ? 0 : 1);
    TEST_EQ(mesh_check_clean(), 0);
  }
  limit = vertices - 2;
  if (limit > triangles / 18 - 1)
    limit = triangles / 18 - 1;
  for (step = 0; step < 2; step++) {
    mesh_reset();
    g_mesh_fail_alloc = 1;
    TEST_EQ(gl_cylinder_init(&cylinder, limit + step, 1), NULL);
    TEST_EQ(g_mesh_buffer_calls, step ? 0 : 2);
    TEST_EQ(g_mesh_init_calls, step ? 0 : 1);
    TEST_EQ(mesh_check_clean(), 0);
  }
}
TEST_CASE_END(gl_mesh_dimension_limits)

#endif

void gl_mesh_test (void);

void gl_mesh_test (void)
{
#if HAVE_GLESV2
  TEST_CASE_RUN(gl_mesh_types);
  TEST_CASE_RUN(gl_mesh_sphere_reject);
  TEST_CASE_RUN(gl_mesh_square_reject);
  TEST_CASE_RUN(gl_mesh_box_reject);
  TEST_CASE_RUN(gl_mesh_cylinder_reject);
  TEST_CASE_RUN(gl_mesh_sphere_small);
  TEST_CASE_RUN(gl_mesh_square_small);
  TEST_CASE_RUN(gl_mesh_box_small);
  TEST_CASE_RUN(gl_mesh_cylinder_small);
  TEST_CASE_RUN(gl_mesh_failures);
  TEST_CASE_RUN(gl_mesh_allocate_reject);
  TEST_CASE_RUN(gl_mesh_allocate_replace);
  TEST_CASE_RUN(gl_mesh_size_limits);
  TEST_CASE_RUN(gl_mesh_dimension_limits);
#endif
}
