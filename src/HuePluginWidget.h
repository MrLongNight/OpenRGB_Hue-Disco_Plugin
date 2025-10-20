#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

// Forward declaration to avoid circular dependencies
class PluginHueEntertainment;

class HuePluginWidget : public QWidget {
    Q_OBJECT

public:
    explicit HuePluginWidget(PluginHueEntertainment* plugin, QWidget *parent = nullptr);
    ~HuePluginWidget();

    void set_bridge_ip(const std::string& ip);
    void set_status_text(const std::string& text);

private slots:
    void on_authenticate_button_clicked();

private:
    void setup_ui();

    PluginHueEntertainment* plugin_; // Pointer to the main plugin logic

    QLabel* ip_label;
    QLineEdit* ip_line_edit;
    QPushButton* authenticate_button;
    QLabel* status_label;
};
