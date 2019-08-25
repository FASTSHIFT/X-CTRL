#include "FileGroup.h"

void DATA(char com)
{
  if (com == 'r')
  {
    L_XMAX = esf.read(0);
    L_XMIN = esf.read(1);
    L_XMP = esf.read(2);
    L_YMAX = esf.read(3);
    L_YMIN = esf.read(4);
    L_YMP = esf.read(5);

    R_XMAX = esf.read(6);
    R_XMIN = esf.read(7);
    R_XMP = esf.read(8);
    R_YMAX = esf.read(9);
    R_YMIN = esf.read(10);
    R_YMP = esf.read(11);

    S_freq = esf.read(12);
    ctrl_md = esf.read(13);
    CTRL_module = esf.read(14);

    posx_mp = esf.read(15);
    posy_mp = esf.read(16);

    CTRL_object = esf.read(17);
    Baud = esf.read(18);
  }
  else if (com == 's')
  {
    esf.write(L_XMAX, 0);
    esf.write(L_XMIN, 1);
    esf.write(L_XMP, 2);
    esf.write(L_YMAX, 3);
    esf.write(L_YMIN, 4);
    esf.write(L_YMP, 5);

    esf.write(R_XMAX, 6);
    esf.write(R_XMIN, 7);
    esf.write(R_XMP, 8);
    esf.write(R_YMAX, 9);
    esf.write(R_YMIN, 10);
    esf.write(R_YMP, 11);

    esf.write(S_freq, 12);
    esf.write(ctrl_md, 13);
    esf.write(CTRL_module, 14);

    esf.write(posx_mp, 15);
    esf.write(posy_mp, 16);

    esf.write(CTRL_object, 17);
    esf.write(Baud, 18);
  }
  else if (com == 'c') esf.clear();
}