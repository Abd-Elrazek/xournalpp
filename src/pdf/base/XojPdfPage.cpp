#include "XojPdfPage.h"

XojPdfRectangle::XojPdfRectangle()
{
    this->x1 = -1;
    this->x2 = -1;
    this->y1 = -1;
    this->y2 = -1;
}

XojPdfRectangle::XojPdfRectangle(double x1, double y1, double x2, double y2)
 : x1(x1),
   y1(y1),
   x2(x2),
   y2(y2)
{
}

XojPdfPage::XojPdfPage()
{
    XOJ_INIT_TYPE(XojPdfPage);
}

XojPdfPage::~XojPdfPage()
{
    XOJ_RELEASE_TYPE(XojPdfPage);
}
