#include "KvOpHelper1d.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>


std::shared_ptr<KvData> KvOpHelper1d::processImpl_(std::shared_ptr<KvData> data)
{
    if (data->dim() == 1) {
        std::shared_ptr<KcSampled1d> res = std::make_shared<KcSampled1d>();
        res->resize(length(0), data->channels());
        res->reset(0, range(0).low(), step(0), 0.5); // TODO: nonuniform
        
        auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);
        if (data->channels() == 1 && data1d) {
            processNaive_(data1d->data(), data1d->count(), (kReal*)res->data());
        }
        else {
            std::vector<kReal> rawData(data->length(0));
            std::vector<kReal> out(length(0));
            for (kIndex c = 0; c < data->channels(); c++) {
                for (kIndex i = 0; i < data->length(0); i++)
                    rawData[i] = data->value(&i, c);

                processNaive_(rawData.data(), rawData.size(), out.data());
                res->setChannel(nullptr, c, out.data());
            }
        }
        
        return res;
    }

    assert(data->dim() == 2);
    auto res = std::make_shared<KcSampled2d>();
    res->resize(data->length(0), length(1), data->channels());
    res->reset(0, range(0).low(), step(0), 0.5);
    res->reset(1, range(1).low(), step(1), 0.5); // TODO: nonuniform

    auto data2d = std::dynamic_pointer_cast<KcSampled2d>(data);
    if (data->channels() == 1 && data2d) {
        for (kIndex i = 0; i < data2d->length(0); i++) 
            processNaive_(data2d->row(i), data2d->length(1), res->row(i));
    }
    else {
        std::vector<kReal> rawData(data->length(1));
        std::vector<kReal> out(length(1));
        for (kIndex c = 0; c < data->channels(); c++) {
            kIndex idx[2];
            for (idx[0] = 0; idx[0] < data->length(0); idx[0]++) {
                for (idx[1] = 0; idx[1] < data->length(1); idx[1]++)
                    rawData[idx[1]] = data->value(idx, c);

                processNaive_(rawData.data(), rawData.size(), out.data());
                res->setChannel(idx, c, out.data());
            }
        }
    }

    return res;
}
