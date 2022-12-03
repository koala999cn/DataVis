#include "KcLayoutOverlay.h"


void KcLayoutOverlay::placeInset(KvLayoutElement* ele, KeAlignment loc)
{
	this->append(ele);
	ele->align() = align;
}
