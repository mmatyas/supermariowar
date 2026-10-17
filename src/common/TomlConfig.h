#pragma once

#include "ConfigNames.h"

#include <toml.hpp>


namespace config {

template <typename S>
toml::ordered_value to_toml_table(const S& s);

template <typename T>
toml::ordered_value to_toml(const T& value)
{
    if constexpr (std::is_same_v<T, bool>) {
        return toml::ordered_value(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return toml::ordered_value(value);
    } else if constexpr (has_named_enum<T>::value) {
        return toml::ordered_value(std::string(as_str(value)));
    } else if constexpr (std::is_arithmetic_v<T>) {
        return toml::ordered_value(value);
    } else if constexpr (is_std_array<T>::value) {
        toml::ordered_array arr;
        for (const auto& element : value)
            arr.push_back(to_toml(element));
        toml::ordered_value result = std::move(arr);
        // arrays of scalars, strings, and enums read better on one line
        if constexpr (std::is_arithmetic_v<typename T::value_type> || std::is_same_v<typename T::value_type, std::string>
                      || std::is_enum_v<typename T::value_type>)
            result.as_array_fmt().fmt = toml::array_format::oneline;
        return result;
    } else if constexpr (has_named_struct<T>::value) {
        return to_toml_table(value);
    } else {
        static_assert(!sizeof(T), "Unsupported field type: add a NamedEnum or NamedStruct specialization");
    }
}

template <typename S>
toml::ordered_value to_toml_table(const S& s)
{
    toml::ordered_table table;
    std::apply([&table, &s](const auto&... f) {
        (table.emplace(std::string(f.name), to_toml(s.*(f.member))), ...);
    }, NamedStruct<std::remove_cv_t<S>>::table);
    return toml::ordered_value(std::move(table));
}


template <typename T>
void from_toml(T& value, const toml::value& t);

/// Reads a field named `key` from a toml table if it exists.
template <typename T>
void read_field(T& value, const toml::value& table, std::string_view key)
{
    try {
        const toml::value& item = table.at(std::string(key));
        from_toml(value, item);
    }
    catch (const std::out_of_range&) {
        // missing key: keep the current value
    }
    catch (const std::exception& error) {
        printf("[config][warning] Failed to read field '%s': %s\n", std::string(key).c_str(), error.what());
    }
}

/// Reads the element at `index` from a toml array if it exists.
template <typename T>
void read_field(T& value, const toml::value& array, std::size_t index)
{
    try {
        const toml::value& item = array.at(index);
        from_toml(value, item);
    }
    catch (const std::exception& error) {
        printf("[config][warning] Failed to read element %zu: %s\n", index, error.what());
    }
}

template <typename S>
void from_toml_table(S& s, const toml::value& table)
{
    if (!table.is_table())
        throw std::runtime_error("expected a table");

    std::apply([&s, &table](const auto&... f) {
        (read_field(s.*(f.member), table, f.name), ...);
    }, NamedStruct<std::remove_cv_t<S>>::table);
}

template <typename T>
void from_toml(T& value, const toml::value& t)
{
    if constexpr (std::is_same_v<T, bool>) {
        value = t.as_boolean();
    } else if constexpr (std::is_same_v<T, std::string>) {
        value = t.as_string();
    } else if constexpr (has_named_enum<T>::value) {
        const std::optional<T> parsed = from_str<T>(t.as_string());
        if (!parsed)
            throw std::runtime_error("unknown value '" + t.as_string() + "'");
        value = *parsed;
    } else if constexpr (std::is_arithmetic_v<T>) {
        value = static_cast<T>(t.as_integer());
    } else if constexpr (std::is_array_v<T>) {
        constexpr std::size_t length = std::extent_v<T>;
        const toml::array& arr = t.as_array();
        if (arr.size() != length)
            throw std::runtime_error("array size mismatch: expected " + std::to_string(length) + " elements");
        for (std::size_t i = 0; i < length; i++)
            from_toml(value[i], arr[i]);
    } else if constexpr (is_std_array<T>::value) {
        const toml::array& arr = t.as_array();
        if (arr.size() != value.size())
            throw std::runtime_error("array size mismatch: expected " + std::to_string(value.size()) + " elements");
        for (std::size_t i = 0; i < value.size(); i++)
            from_toml(value[i], arr[i]);
    } else if constexpr (has_named_struct<T>::value) {
        from_toml_table(value, t);
    } else {
        static_assert(!sizeof(T), "Unsupported field type: add a NamedEnum or NamedStruct specialization");
    }
}


template <typename Struct>
toml::ordered_value make_config_document(const Struct& config, const Bridge& bridge)
{
    toml::ordered_table root;
    std::apply([&root, &config, &bridge](const auto&... sec) {
        auto add_section = [&](const auto& sec) {
            if constexpr (requires { sec.fields; }) {
                toml::ordered_table table;
                std::apply([&table, &config, &bridge](const auto&... f) {
                    (table.emplace(std::string(f.name), to_toml(field_value(f.member, config, bridge))), ...);
                }, sec.fields);
                root.emplace(std::string(sec.name), toml::ordered_value(std::move(table)));
            } else {
                toml::ordered_value value = to_toml_table(field_value(sec.member, config, bridge));
                // Skip the header for a table of tables (eg. gamemode_settings)
                // Keep it for plain tables (eg. gamemode_goals)
                if (value.is_table() && std::all_of(value.as_table().begin(), value.as_table().end(),
                        [](const auto& item) { return item.second.is_table(); }))
                    value.as_table_fmt().fmt = toml::table_format::implicit;
                root.emplace(std::string(sec.name), std::move(value));
            }
        };
        (add_section(sec), ...);
    }, NamedLayout<std::remove_cv_t<Struct>>::sections);
    return toml::ordered_value(std::move(root));
}


template <typename Struct>
void apply_config_document(Struct& config, Bridge& bridge, const toml::value& document)
{
    if (!document.is_table())
        return;

    std::apply([&config, &bridge, &document](const auto&... sec) {
        auto read_section = [&](const auto& sec) {
            if (!document.contains(std::string(sec.name)))
                return;
            const toml::value& table = document.at(std::string(sec.name));

            if constexpr (requires { sec.fields; }) {
                std::apply([&table, &config, &bridge](const auto&... f) {
                    (read_field(field_value(f.member, config, bridge), table, f.name), ...);
                }, sec.fields);
            } else {
                if (table.is_table())
                    from_toml(field_value(sec.member, config, bridge), table);
            }
        };
        (read_section(sec), ...);
    }, NamedLayout<std::remove_cv_t<Struct>>::sections);
}

} // namespace config
