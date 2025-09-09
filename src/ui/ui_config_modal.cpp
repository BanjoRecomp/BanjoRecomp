#include "ui_config_modal.h"
#include "config/ui_config_tab_graphics.h"
#include "config/ui_config_tab_controls.h"
#include "banjo_config.h"


namespace recompui {

Modal *config_modal = nullptr;



static recomp::config::Config example_manifest("Example Config", "example_config", true);
static ConfigTab example_tab(&example_manifest);

static recomp::config::Config example_manifest2("No apply", "example_config_no_apply", false);
static ConfigTab example_tab2(&example_manifest2);

// ConfigTabGeneral config_tab_general = ConfigTabGeneral();

static std::vector<TabContext *> config_tabs = {
    // &config_tab_general,
    &config_tab_graphics,
    &config_tab_controls,
    // other hardcoded tabs
};

enum class TheThings {
    This,
    That,
    TheOther
};

const std::vector<recomp::config::ConfigOptionEnumOption> example_enum_options = {
    {TheThings::TheOther, "theotherr", "The Other ñ"},
    {TheThings::That, "that", "That"},
    {TheThings::This, "this", "This"},
};

enum class TheUnsureThings {
    Yes,
    Maybe,
    Unsure,
    No,
    Nah,

    size
};

const std::vector<recomp::config::ConfigOptionEnumOption> example_enum2_options = {
    {TheUnsureThings::Yes, "Yes?"},
    {TheUnsureThings::Maybe, "Maybe"},
    {TheUnsureThings::Unsure, "Unsure"},
    {TheUnsureThings::No, "No"},
    {TheUnsureThings::Nah, "Nah"}
};

// typedef std::variant<std::monostate, uint32_t, double, std::string> ConfigValueVariant;
void print_option_value_change_s(const std::string &option_id, std::string value, std::string prev_value) {
    printf("Option '%s' changed from '%s' to '%s'\n",
        option_id.c_str(),
        prev_value.c_str(),
        value.c_str()
    );
}
void print_option_value_change_i(const std::string &option_id, int value, int prev_value) {
    printf("Option '%s' changed from '%d' to '%d'\n",
        option_id.c_str(),
        prev_value,
        value
    );
}
void print_option_value_change_f(const std::string &option_id, double value, double prev_value) {
    printf("Option '%s' changed from '%f' to '%f'\n",
        option_id.c_str(),
        prev_value,
        value
    );
}

void init_example_config() {
    using OptionChangeContext = recomp::config::OptionChangeContext;
    example_manifest.add_number_option(
        "example_option_number",
        "Rumble strength",
        "This is just a little example hehe",
        0.0,
        10.0,
        1.0,
        0,
        false,
        5
    );
    example_manifest.add_option_change_callback("example_option_number", [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
        print_option_value_change_f("example_option_number", std::get<double>(value), std::get<double>(prev_value));
    });
    example_manifest.add_enum_option(
        "example_option_enum",
        "Example Option Enum",
        "This is just another little example haha!",
        example_enum_options,
        TheThings::TheOther
    );
    example_manifest.add_bool_option(
        "example_option_bool",
        "Option Bool",
        "Just a stupid fucking bool",
        true
    );
    example_manifest.add_bool_option(
        "example_option_bool2",
        "Option Bool 2",
        "Just another stupid fucking bool",
        false
    );

    // example_manifest.add_option_change_callback("example_option_enum", [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant prev_value) {
    //     print_option_value_change_i("example_option_enum", std::get<uint32_t>(value), std::get<uint32_t>(prev_value));
    //     TheThings new_value = static_cast<TheThings>(std::get<uint32_t>(value));
    //     if (new_value == TheThings::This) {
    //         example_manifest.set_option_value("example_option_enum2", static_cast<uint32_t>(TheUnsureThings::Yes));
    //         example_manifest.update_option_enum_details("example_option_enum2", "");
    //         example_manifest.update_option_disabled("example_option_enum2", false);
    //         example_manifest.update_option_hidden("example_option_string", false);
    //     }
    //     if (new_value == TheThings::That) {
    //         example_manifest.update_option_enum_details("example_option_enum2", "surprise bitch");
    //         example_manifest.update_option_disabled("example_option_enum2", false);
    //         example_manifest.update_option_hidden("example_option_string", true);
    //     }
    //     if (new_value == TheThings::TheOther) {
    //         example_manifest.update_option_enum_details("example_option_enum2", "im disabled uwu");
    //         example_manifest.update_option_disabled("example_option_enum2", true);
    //         example_manifest.update_option_description("example_option_enum", "we did this on the flyyyyy");
    //         example_manifest.update_option_hidden("example_option_string", false);
    //     }
    // });
    example_manifest.add_string_option(
        "example_option_string",
        "Example Option String",
        "string example...! description........!!!!",
        "defaultma"
    );
    example_manifest.add_option_change_callback("example_option_string", [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
        print_option_value_change_s("example_option_string", std::get<std::string>(value), std::get<std::string>(prev_value));
    });
    example_manifest.add_enum_option(
        "example_option_enum2",
        "Choosy",
        "choosyyyyyy",
        example_enum2_options,
        TheUnsureThings::Unsure
    );
    example_manifest.add_option_change_callback("example_option_enum2", [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
        print_option_value_change_i("example_option_enum2", std::get<uint32_t>(value), std::get<uint32_t>(prev_value));
    });
    example_manifest.set_apply_callback([]() {
        printf("Example config applied!\n");
    });

    for (int i = 2; i < 10; ++i) {
        example_manifest.add_string_option(
            "example_option_string" + std::to_string(i),
            "Example Option String " + std::to_string(i),
            "string example...! description........!!!!",
            "defaultma"
        );
    }

    example_manifest.add_option_disable_dependency(
        "example_option_enum",
        "example_option_enum2",
        TheUnsureThings::Unsure
    );

    example_manifest.add_option_hidden_dependency(
        "example_option_string2",
        "example_option_enum2",
        TheUnsureThings::Unsure
    );
    example_manifest.add_option_hidden_dependency(
        "example_option_string3",
        "example_option_enum2",
        TheUnsureThings::Unsure
    );
    example_manifest.add_option_hidden_dependency(
        "example_option_string4",
        "example_option_enum2",
        TheUnsureThings::Unsure
    );

    example_manifest.load_config();

    example_manifest2.add_number_option(
        "example_option_number_no_apply",
        "Example Option Number No Apply",
        "words words words words PUNCHLINE",
        0.0,
        10.0,
        1.0,
        0,
        false,
        5
    );
    example_manifest2.add_option_change_callback("example_option_number_no_apply", [](recomp::config::ConfigValueVariant value, recomp::config::ConfigValueVariant prev_value, OptionChangeContext change_context) {
        print_option_value_change_f("example_option_number_no_apply", std::get<double>(value), std::get<double>(prev_value));
    });

    example_manifest2.load_config();

    add_config_modal_tab(&example_tab);
    add_config_modal_tab(&example_tab2);
}

void add_config_modal_tab(TabContext *tab, bool add_to_front) {
    if (add_to_front) {
        config_tabs.insert(config_tabs.begin(), tab);
    } else {
        config_tabs.push_back(tab);
    }
}

void init_config_modal() {
    init_example_config();

    config_modal = Modal::create_modal(ModalType::Fullscreen);
    config_modal->modal_root_context.open();
    for (auto tab : config_tabs) {
        config_modal->add_tab(tab);
    }
    config_modal->set_selected_tab(0);
    config_modal->set_on_close_callback([]() {
        banjo::save_config();
    });
    config_modal->modal_root_context.close();
}

} // namespace recompui
