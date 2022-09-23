#pragma once
#include "KvImWindow.h"


class KcImNodeEditor : public KvImWindow
{
public:
    explicit KcImNodeEditor(const std::string_view& name);

    const char* id() const override { return "NodeEditor"; }

    void draw() override;

private:

};
