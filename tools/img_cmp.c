#include <gegl.h> 
#include <math.h>

gint
main (gint    argc,
      gchar **argv)
{
  GeglBuffer *bufferA = NULL;
  GeglBuffer *bufferB = NULL;

  gegl_init (&argc, &argv);

  if (argc != 3)
    {
      g_print ("This is simple image difference detection tool for use in regression testing"
               "return message is non zero if images are different, if they are equal"
               "the output will contain the string identical.");
      g_print ("Usage: %s <imageA> <imageB>\n", argv[0]);
      return 1;
    }

  {
    GeglNode *graph, *sink;
    graph = gegl_graph (sink=gegl_node ("gegl:buffer-sink", "buffer", &bufferA, NULL,
                             gegl_node ("gegl:load", "path", argv[1], NULL)));
    gegl_node_process (sink);
    g_object_unref (graph);
    if (!bufferA)
      {
        g_print ("Failed top open %s\n", argv[1]);
        return 1;
      }

    graph = gegl_graph (sink=gegl_node ("gegl:buffer-sink", "buffer", &bufferB, NULL,
                             gegl_node ("gegl:load", "path", argv[2], NULL)));
    gegl_node_process (sink);
    g_object_unref (graph);
    if (!bufferB)
      {
        g_print ("Failed top open %s\n", argv[2]);
        return 1;
      }
  }

  if (gegl_buffer_get_width (bufferA) != gegl_buffer_get_width (bufferB) ||
      gegl_buffer_get_height (bufferA) != gegl_buffer_get_height (bufferB))
    {
      g_print ("%s and %s differ in size\n", argv[1], argv[2]);
      g_print ("  %ix%i vs %ix%i\n",
        gegl_buffer_get_width (bufferA), gegl_buffer_get_height (bufferA),
        gegl_buffer_get_width (bufferB), gegl_buffer_get_height (bufferB));
      return 1;
    }

  {
     gfloat *bufA, *bufB;
     gfloat *a, *b;
     gint   rowstrideA, rowstrideB;
     gint   pixels;
     gint   wrong_pixels=0;
     gint   i;
     gdouble diffsum = 0.0;
     gdouble max_diff = 0.0;

     pixels = gegl_buffer_get_pixel_count (bufferA);

     bufA = (void*)gegl_buffer_linear_open (bufferA, NULL, &rowstrideA, babl_format ("CIE Lab float"));
     bufB = (void*)gegl_buffer_linear_open (bufferB, NULL, &rowstrideB, babl_format ("CIE Lab float"));

     a = bufA;
     b = bufB;

     for (i=0;i<pixels;i++)
       {
#define P2(o) (((o))*((o)))
         gdouble diff = sqrt ( P2(a[0]-b[0])+P2(a[1]-b[1])+P2(a[2]-b[2])/*+P2(a[3]-b[3])*/);
#undef P2
         a+=3;
         b+=3;
         if (diff>=0.01)
           {
             wrong_pixels++;
             diffsum += diff;
             if (diff > max_diff)
               max_diff = diff;
           }
       }

     if (max_diff >= 0.1)
       {
         g_print ("%s and %s differ\n"
                  "  wrong pixels   : %i/%i (%2.2f%%)\n"
                  "  max Δe         : %2.3f\n"
                  "  avg Δe (wrong) : %2.3f(wrong) %2.3f(total)\n",
                  argv[1], argv[2],
                  wrong_pixels, pixels, (wrong_pixels*100.0/pixels),
                  max_diff,
                  diffsum/wrong_pixels,
                  diffsum/pixels);
         if (max_diff > 1.5)
           return 1;
         g_print ("we'll say ");
       }

     gegl_buffer_linear_close (bufferA, bufA);
     gegl_buffer_linear_close (bufferB, bufB);
  }

  g_print ("%s and %s are identical\n", argv[1], argv[2]);
  g_object_unref (bufferA); 
  g_object_unref (bufferB); 
  gegl_exit ();
  return 0;
}