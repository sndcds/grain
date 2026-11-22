#pragma once

#include <grain/UI/LayoutStyle.hpp>


namespace Grain {

class UIStyles {
public:
    void defineViewStyle(
        const std::string& name,
        const LayoutViewStyle& style
    ) {
        view_styles_.insert_or_assign(name, style);
    }

    void defineComponentStyle(
        const std::string& name,
        const LayoutComponentStyle& style
    ) {
        component_styles_.insert_or_assign(name, style);
    }

    [[nodiscard]]
    auto viewStyle(const std::string& name) const
        -> const LayoutViewStyle&
    {
        return view_styles_.at(name);
    }

    [[nodiscard]]
    auto componentStyle(const std::string& name) const
        -> const LayoutComponentStyle&
    {
        return component_styles_.at(name);
    }

private:
    std::map<std::string, LayoutViewStyle> view_styles_;
    std::map<std::string, LayoutComponentStyle> component_styles_;
};

}