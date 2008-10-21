/* This file is part of GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2007 Øyvind Kolås <pippin@gimp.org>
 */

#ifndef __GEGL_PATH_H__
#define __GEGL_PATH_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GEGL_TYPE_PATH            (gegl_path_get_type ())
#define GEGL_PATH(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEGL_TYPE_PATH, GeglPath))
#define GEGL_PATH_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GEGL_TYPE_PATH, GeglPathClass))
#define GEGL_IS_PATH(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_PATH))
#define GEGL_IS_PATH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GEGL_TYPE_PATH))
#define GEGL_PATH_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GEGL_TYPE_PATH, GeglPathClass))

typedef struct _GeglPathClass  GeglPathClass;

struct _GeglPath
{
  GObject parent_instance;
};

/* Internally the following structures are used, parts
 * of the internal implementation are exposed through
 * the path access API. The linked list api is currently
 * only used for adding new path interpolators/flatteners
 * with new knot interpretations.
 */

typedef struct Point
{
  gfloat x;
  gfloat y;
} Point;

typedef struct GeglPathItem
{
  gchar  type; /* should perhaps be padded out? */
  Point  point[4];
} GeglPathItem;


GType                gegl_path_get_type       (void) G_GNUC_CONST;
GeglPath           * gegl_path_new            (void);

void                 gegl_path_parse_string   (GeglPath    *path,
                                               const gchar *path_string);
gchar              * gegl_path_to_string      (GeglPath    *path);

gint                 gegl_path_get_count      (GeglPath    *path);

void                 gegl_path_clear          (GeglPath    *path);
void                 gegl_path_append         (GeglPath    *self,
                                                            ...);
void                 gegl_path_insert         (GeglPath    *path,
                                               gint         pos,
                                               const GeglPathItem *knot);
const GeglPathItem * gegl_path_get            (GeglPath    *path,
                                               gint         pos);
void                 gegl_path_replace        (GeglPath    *path,
                                               gint         pos,
                                               const GeglPathItem *knot);
void                 gegl_path_remove         (GeglPath    *path,
                                               gint         pos);

void                 gegl_path_foreach        (GeglPath    *path,
                                               void       (*each_item) (
                                                     const GeglPathItem *knot,
                                                     gpointer            data),
                                               gpointer     data);
void                 gegl_path_foreach_flat   (GeglPath   *path,
                                               void       (*each_item) (
                                                     const GeglPathItem *knot,
                                                     gpointer            data),
                                               gpointer     data);

gdouble              gegl_path_get_length     (GeglPath     *self);
void                 gegl_path_calc           (GeglPath     *path,
                                               gdouble       pos,
                                               gdouble      *dest_x,
                                               gdouble      *dest_y);
void                 gegl_path_get_bounds     (GeglPath     *self,
                                               gdouble      *min_x,
                                               gdouble      *max_x,
                                               gdouble      *min_y,
                                               gdouble      *max_y);
void                 gegl_path_calc_values    (GeglPath    *self,
                                               guint        num_samples,
                                               gdouble     *dest_xs,
                                               gdouble     *dest_ys);

GParamSpec         * gegl_param_spec_path     (const gchar *name,
                                               const gchar *nick,
                                               const gchar *blurb,
                                               GeglPath    *default_path,
                                               GParamFlags  flags);

#define GEGL_TYPE_PARAM_PATH    (gegl_param_path_get_type ())
#define GEGL_IS_PARAM_PATH(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEGL_TYPE_PARAM_PATH))
GType                gegl_param_path_get_type (void) G_GNUC_CONST;


/* Linked list used internally, and for the plug-in API for new path
 * interpolators.
 */
typedef struct GeglPathList
{
  GeglPathItem         d;
  struct GeglPathList *next;
} GeglPathList;

/* appends to path list, if head is NULL a new list is created */
GeglPathList       * gegl_path_list_append    (GeglPathList *head, ...);
/* frees up a path list */
GeglPathList       * gegl_path_list_destroy   (GeglPathList *path);


/* add a new control point type to GeglPath, this new type works in everything
 * from the _add () functions (number of arguments determined automatically)
 * to the string parsing and serialization functions.
 */
void gegl_path_add_type (gchar        type,
                         gint         pairs,
                         const gchar *description);

/* Add a new flattener, the flattener should produce a type of path that
 * GeglPath already understands, if the flattener is unable to flatten
 * the incoming path (doesn't understand the instructions), the original
 * path should be returned.
 */
void gegl_path_add_flattener (GeglPathList *(*func) (GeglPathList *original));


#if 0
const GeglMatrix *gegl_path_get_matrix (GeglPath *path);
GeglMatrix gegl_path_set_matrix (GeglPath *path,
                                   const GeglMatrix *matrix);
#endif

#include <gegl-buffer.h>

/* this can and should be the responsiblity of cairo */
void gegl_path_fill (GeglBuffer *buffer,
                     GeglPath   *path,
                     GeglColor  *color,
                     gboolean    winding);

/* Stroke the path with a brush having the specified attributes
 * (code from horizon)
 */
void gegl_path_stroke (GeglBuffer *buffer,
                       GeglPath   *path,
                       GeglColor  *color,
                       gdouble     linewidth,
                       gdouble     hardness);



G_END_DECLS

#endif /* __GEGL_PATH_H__ */