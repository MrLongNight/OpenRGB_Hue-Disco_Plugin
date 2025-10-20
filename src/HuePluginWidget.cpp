#include "HuePluginWidget.h"
#include "PluginHueEntertainment.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

HuePluginWidget::HuePluginWidget(PluginHueEntertainment* plugin, QWidget *parent) :
    plugin_(plugin), QWidget(parent)
{
    setup_ui();
    connect(authenticate_button, &QPushButton::clicked, this, &HuePluginWidget::on_authenticate_button_clicked);
}

HuePluginWidget::~HuePluginWidget() {}

void HuePluginWidget::setup_ui() {
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QHBoxLayout* ip_layout = new QHBoxLayout();
    ip_label = new QLabel("Bridge IP:", this);
    ip_line_edit = new QLineEdit(this);
    ip_line_edit->setReadOnly(true); // IP is discovered automatically
    ip_layout->addWidget(ip_label);
    ip_layout->addWidget(ip_line_edit);

    authenticate_button = new QPushButton("Authenticate with Bridge", this);
    status_label = new QLabel("Status: Initializing...", this);

    main_layout->addLayout(ip_layout);
    main_layout->addWidget(authenticate_button);
    main_layout->addWidget(status_label);

    setLayout(main_layout);
}

void HuePluginWidget::set_bridge_ip(const std::string& ip) {
    ip_line_edit->setText(QString::fromStdString(ip));
}

void HuePluginWidget::set_status_text(const std::string& text) {
    status_label->setText(QString::fromStdString(text));
}

void HuePluginWidget::on_authenticate_button_clicked() {
    status_label->setText("Status: Press the button on your Hue Bridge...");
    // Trigger the authentication process in the main plugin
    plugin_->start_pushlink_authentication();
}
