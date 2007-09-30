if test "x$PREFIX" = "x"; then
    PREFIX=/usr/local
fi

CODE_GEN=/usr/share/pygtk/2.0/codegen/h2def.py
INCLUDE=$PREFIX/include/gegl-1.0

NEWDEFS=gegl.defs_`date --rfc-3339=date`

#python $CODE_GEN `grep include $INCLUDE/gegl.h | grep gegl | sed -e "s@.*<\(.*\)>@$INCLUDE/\1@"` > out.defs
python $CODE_GEN $INCLUDE/gegl.h $INCLUDE/gegl/gegl-utils.h > out.defs

gvim -d out.defs gegl.defs
