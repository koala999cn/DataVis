#include "KcModuleImNode.h"
#include "imnodes/imnodes.h"


KcModuleImNode::KcModuleImNode()
    : KvModule("ImNode")
{

}


bool KcModuleImNode::initialize()
{
    if (ImNodes::CreateContext() == nullptr)
        return false;

    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));

    return true;
}


void KcModuleImNode::deinitialize()
{
    ImNodes::DestroyContext();
}
