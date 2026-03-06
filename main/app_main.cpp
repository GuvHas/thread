#include <cinttypes>
#include <cstring>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_matter.h"
#include <app/server/Server.h>
#include <app-common/zap-generated/attributes/Accessors.h>

using namespace esp_matter;
using namespace esp_matter::attribute;

static const char *TAG = "xiao_matter_thread";


static esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    if (type == PRE_UPDATE) {
        ESP_LOGI(TAG,
                 "Attribute update: endpoint=%u cluster=0x%08" PRIx32 " attribute=0x%08" PRIx32,
                 endpoint_id, cluster_id, attribute_id);
    }
    return ESP_OK;
}

static esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id,
                                       uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    ESP_LOGI(TAG, "Identification callback type=%d endpoint=%u effect_id=%u variant=%u",
             type, endpoint_id, effect_id, effect_variant);
    return ESP_OK;
}

static void set_node_label_if_present(node_t *node)
{
    endpoint_t *root = endpoint::get(node, 0);
    if (!root) {
        ESP_LOGW(TAG, "Root endpoint not found; node label left unchanged");
        return;
    }

    cluster_t *basic_info = cluster::get(root, chip::app::Clusters::BasicInformation::Id);
    if (!basic_info) {
        ESP_LOGW(TAG, "Basic Information cluster missing; node label left unchanged");
        return;
    }

    attribute_t *node_label =
        attribute::get(basic_info, chip::app::Clusters::BasicInformation::Attributes::NodeLabel::Id);
    if (!node_label) {
        ESP_LOGW(TAG, "NodeLabel attribute missing; node label left unchanged");
        return;
    }

    char label[33] = {0};
    std::strncpy(label, CONFIG_MATTER_NODE_LABEL, sizeof(label) - 1);
    esp_matter_attr_val_t label_val = esp_matter_char_str(label, std::strlen(label));
    ESP_ERROR_CHECK(attribute::update(0, node_label, &label_val));

    ESP_LOGI(TAG, "Matter NodeLabel set to '%s'", label);
}

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Matter commissioning completed");
        break;
    case chip::DeviceLayer::DeviceEventType::kThreadStateChange:
        ESP_LOGI(TAG, "Thread state changed");
        break;
    default:
        break;
    }
}

extern "C" void app_main()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    if (!node) {
        ESP_LOGE(TAG, "Failed to create Matter node");
        abort();
    }

    endpoint::on_off_light::config_t light_config;
    endpoint_t *light = endpoint::on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE, nullptr);
    if (!light) {
        ESP_LOGE(TAG, "Failed to create On/Off light endpoint");
        abort();
    }

    set_node_label_if_present(node);

    ESP_ERROR_CHECK(esp_matter::start(app_event_cb));

    ESP_LOGI(TAG, "Matter over Thread started. Use commissioner app to add to existing Thread network.");
    ESP_LOGI(TAG, "Power from USB-C, hold BOOT during reset for download mode when flashing.");
}
