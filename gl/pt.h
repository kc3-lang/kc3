#ifndef LIBKC3_GL_PT_H
#define LIBKC3_GL_PT_H

#include "../image/types.h"
#include "types.h"

s_image ** pt (s_image **image, p_list *scene);

/* Intersection. */
f64 pt_intersect_sphere (const s_pt_sphere *s, const s_dray *r);

/* Radiance. */
s_dvec3 * pt_radiance (const s_list *scene, const s_dray *ray,
                       u32 depth, u16 *xi, s_dvec3 *dest);
s_dvec3 * pt_radiance_sphere (const s_list *scene,
                              const s_dray *ray,
                              u32 depth, u16 *xi, s_pt_sphere *obj,
                              f64 dist, s_dvec3 *dest);

#endif /* LIBKC3_PT_H */
