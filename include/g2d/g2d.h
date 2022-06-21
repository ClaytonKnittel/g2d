#ifndef _G2D_H
#define _G2D_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef USE_OPENGL

#include <g2d/opengl/gl.h>

#elif defined(USE_METAL)

#include <g2d/metal/metal.h>

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _G2D_H */
