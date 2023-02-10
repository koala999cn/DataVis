#include "KcOpKde.h"
#include "dsp/KtKde.h"
#include "KvDiscreted.h"
#include "KcContinuedFn.h"
#include "imguix.h"


KcOpKde::KcOpKde()
    : super_("KDE")
{

}


int KcOpKde::spec(kIndex outPort) const
{
    KpDataSpec sp(super_::spec(outPort));
    sp.type = k_continued;
    return sp.spec;
}


kReal KcOpKde::step(kIndex outPort, kIndex axis) const
{
    assert(axis < dim(outPort));
    return 0;
}


kIndex KcOpKde::size(kIndex outPort, kIndex axis) const
{
    assert(axis < dim(outPort));
    return KvData::k_inf_size;
}


kRange KcOpKde::range(kIndex outPort, kIndex axis) const
{
    return odata_.front() ? odata_.front() ->range(axis) : kRange{ 0, 1 };
}


bool KcOpKde::permitInput(int dataSpec, unsigned inPort) const
{
    assert(inPort == 0);
    KpDataSpec sp(dataSpec);
    return sp.type != k_continued && sp.dim == 1 && !sp.dynamic && !sp.stream;
}


void KcOpKde::showPropertySet()
{
    super_::showPropertySet();

    if (odata_.front()) {
        auto cont = std::dynamic_pointer_cast<KvContinued>(odata_.front());

        ImGui::Separator();

        if (ImGuiX::treePush("Range", true)) {
            char label[] = { 'X', '\0' };
            for (unsigned i = 0; i < cont->dim(); i++) {
                auto r = cont->range(i);
                float low = r.low(), high = r.high();
                if (ImGui::DragFloatRange2(label, &low, &high) && high > low) {
                    cont->setRange(i, low, high);
                    notifyChanged(); // 由于此处直接更新输出数据，所以调用notifyChanged
                }

                label[0] += 1;
            }

            ImGuiX::treePop();
        }
    }
}


void KcOpKde::outputImpl_()
{
    if (idata_.front()) {
        auto disc = std::dynamic_pointer_cast<KvDiscreted>(idata_.front());

        auto getter = [disc](unsigned i) {
            return disc->valueAt(i, 0); // TODO: 多通道支持
        };

        auto kernel = [](kReal t) {
            return (t >= 0 && t <= 1) ? (3. / KuMath::pi) * std::pow(1. - t * t, 2.) : 0;
        };

        auto kde = KtKde<kReal>(getter, disc->size(), kernel);
        auto d = std::make_shared<KcContinuedFn>([kde](kReal x) { return kde(x); });
        auto r = disc->valueRange(0); // TODO: 多通道支持
        d->setRange(0, r.low(), r.high());
        odata_.front() = d;
    }
    else {
        odata_.front() = nullptr;
    }
}
