#include "ButtonConfigGui.h"

#include "SettingsDialog.h"
#include "control/settings/Settings.h"
#include "control/settings/ButtonConfig.h"
#include "util/DeviceListHelper.h"

#include <config.h>
#include <i18n.h>
#include <Util.h>

#define	ADD_TYPE_CB(icon, name, action) \
	gtk_list_store_append(typeModel, &iter); \
	gtk_list_store_set(typeModel, &iter, 0, dlg->loadIconPixbuf(icon), 1, name, 2, action, -1);

ButtonConfigGui::ButtonConfigGui(SettingsDialog* dlg, GladeSearchpath* gladeSearchPath, GtkWidget* w,  Settings* settings, int button, bool withDevice)
 : GladeGui(gladeSearchPath, "settingsButtonConfig.glade", "offscreenwindow")
{
	XOJ_INIT_TYPE(ButtonConfigGui);

	this->settings = settings;
	this->button = button;
	this->withDevice = withDevice;

	GtkWidget* mainGrid = get("mainGrid");
	gtk_container_remove(GTK_CONTAINER(getWindow()), mainGrid);
	gtk_container_add(GTK_CONTAINER(w), mainGrid);
	gtk_widget_show_all(mainGrid);

	this->cbDevice = get("cbDevice");
	this->cbDisableDrawing = get("cbDisableDrawing");

	if (withDevice)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDevice), _("No device"));

		DeviceListHelper devList;
		for (InputDevice& dev : devList.getDeviceList())
		{
			string txt = dev.getName()  + " (" + dev.getType() + ")";
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDevice), txt.c_str());
		}
	}
	else
	{
		gtk_widget_hide(get("lbDevice"));
		gtk_widget_hide(this->cbDevice);
		gtk_widget_hide(this->cbDisableDrawing);
	}

	GtkListStore* typeModel = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	GtkTreeIter iter;

	ADD_TYPE_CB("empty.svg",            _("Don't change"),     TOOL_NONE);
	ADD_TYPE_CB("tool_pencil.svg",      _("Pen"),              TOOL_PEN);
	ADD_TYPE_CB("tool_eraser.svg",      _("Eraser"),           TOOL_ERASER);
	ADD_TYPE_CB("tool_highlighter.svg", _("Highlighter"),      TOOL_HILIGHTER);
	ADD_TYPE_CB("tool_text.svg",        _("Text"),             TOOL_TEXT);
	ADD_TYPE_CB("tool_image.svg",       _("Insert image"),     TOOL_IMAGE);
	ADD_TYPE_CB("stretch.svg",          _("Vertical space"),   TOOL_VERTICAL_SPACE);
	ADD_TYPE_CB("lasso.svg",            _("Select region"),    TOOL_SELECT_REGION);
	ADD_TYPE_CB("rect-select.svg",      _("Select rectangle"), TOOL_SELECT_RECT);
	//ADD_TYPE_CB("rect-draw.svg",      _("Draw rectangle"),   TOOL_DRAW_RECT);
	//ADD_TYPE_CB("circle-draw.svg",    _("Draw circle"),      TOOL_DRAW_CIRCLE);
	ADD_TYPE_CB("hand.svg",             _("Hand"),             TOOL_HAND);


	this->cbTool = get("cbTool");
	gtk_combo_box_set_model(GTK_COMBO_BOX(this->cbTool), GTK_TREE_MODEL(typeModel));
	g_signal_connect(cbTool, "changed", G_CALLBACK(&cbSelectCallback), this);

	GtkCellRenderer* renderer = gtk_cell_renderer_pixbuf_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(this->cbTool), renderer, false);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(this->cbTool), renderer, "pixbuf", 0, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(this->cbTool), renderer, true);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(this->cbTool), renderer, "text", 1, NULL);

	this->cbThickness = get("cbThickness");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbThickness), _("Don't change"));
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbThickness), _("Thin"));
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbThickness), _("Medium"));
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cbThickness), _("Thick"));
	gtk_combo_box_set_active(GTK_COMBO_BOX(cbThickness), 0);

	this->colorButton = get("colorButton");

	this->cbDrawingType = get("cbDrawingType");
	// DRAWING_TYPE_DONT_CHANGE
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Don't change"));
	// DRAWING_TYPE_DEFAULT
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Normal drawing"));
	// DRAWING_TYPE_RULER
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Draw Line"));
	// DRAWING_TYPE_RECTANGLE
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Draw Rectangle"));
	// DRAWING_TYPE_CIRCLE
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Draw Circle"));
	// DRAWING_TYPE_ARROW
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Draw Arrow"));
	// DRAWING_TYPE_STROKE_RECOGNIZER
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->cbDrawingType), _("Stroke recognizer"));


	// Values in glade GUI!
	this->cbEraserType = get("cbEraserType");

	loadSettings();
}

ButtonConfigGui::~ButtonConfigGui()
{
	XOJ_RELEASE_TYPE(ButtonConfigGui);
}

void ButtonConfigGui::loadSettings()
{
	XOJ_CHECK_TYPE(ButtonConfigGui);

	ButtonConfig* cfg = settings->getButtonConfig(button);

	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(cbTool));
	GtkTreeIter iter;

	if (!gtk_tree_model_get_iter_first(model, &iter))
	{
		return;
	}

	GValue value = {0};
	int i = 0;

	gtk_combo_box_set_active(GTK_COMBO_BOX(cbTool), 0);

	do
	{
		gtk_tree_model_get_value(model, &iter, 2, &value);

		int action = g_value_get_int(&value);

		if (action == cfg->action)
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(cbTool), i);
			break;
		}

		i++;
		g_value_unset(&value);
	}
	while (gtk_tree_model_iter_next(model, &iter));

	if (cfg->size == TOOL_SIZE_FINE)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbThickness), 1);
	}
	else if (cfg->size == TOOL_SIZE_MEDIUM)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbThickness), 2);
	}
	else if (cfg->size == TOOL_SIZE_THICK)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbThickness), 3);
	}
	else
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbThickness), 0);
	}


	GdkRGBA color;
	Util::apply_rgb_togdkrgba(color, cfg->color);
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(colorButton), &color);

	gtk_combo_box_set_active(GTK_COMBO_BOX(this->cbDrawingType), cfg->drawingType);

	if (cfg->eraserMode == ERASER_TYPE_DEFAULT)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbEraserType), 1);
	}
	else if (cfg->eraserMode == ERASER_TYPE_WHITEOUT)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbEraserType), 2);
	}
	else if (cfg->eraserMode == ERASER_TYPE_DELETE_STROKE)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbEraserType), 3);
	}
	else
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbEraserType), 0);
	}

	if (withDevice)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(cbDevice), 0);

		int i = 0;


		DeviceListHelper devList;
		for (InputDevice& dev : devList.getDeviceList())
		{
			if (cfg->device == dev.getName())
			{
				gtk_combo_box_set_active(GTK_COMBO_BOX(cbDevice), i + 1);
				break;
			}

			i++;
		}

		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbDisableDrawing), cfg->disableDrawing);
	}
}

// Not implemented! This is not a dialog!
void ButtonConfigGui::show(GtkWindow* parent)
{
}

void ButtonConfigGui::saveSettings()
{
	XOJ_CHECK_TYPE(ButtonConfigGui);

	ButtonConfig* cfg = settings->getButtonConfig(button);
	ToolType action = TOOL_NONE;
	GtkTreeIter iter;

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(cbTool), &iter);

	GValue value = {0};
	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(cbTool));

	gtk_tree_model_get_value(model, &iter, 2, &value);
	action = (ToolType) g_value_get_int(&value);

	cfg->action = action;

	int thickness = gtk_combo_box_get_active(GTK_COMBO_BOX(cbThickness));

	if (thickness == 1)
	{
		cfg->size = TOOL_SIZE_FINE;
	}
	else if (thickness == 2)
	{
		cfg->size = TOOL_SIZE_MEDIUM;
	}
	else if (thickness == 3)
	{
		cfg->size = TOOL_SIZE_THICK;
	}
	else
	{
		cfg->size = TOOL_SIZE_NONE;
	}

	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(colorButton), &color);
	cfg->color = Util::gdkrgba_to_hex(color);

	cfg->drawingType = (DrawingType) gtk_combo_box_get_active(GTK_COMBO_BOX(this->cbDrawingType));

	int eraserMode = gtk_combo_box_get_active(GTK_COMBO_BOX(this->cbEraserType));

	if (eraserMode == 1)
	{
		cfg->eraserMode = ERASER_TYPE_DEFAULT;
	}
	else if (eraserMode == 2)
	{
		cfg->eraserMode = ERASER_TYPE_WHITEOUT;
	}
	else if (eraserMode == 3)
	{
		cfg->eraserMode = ERASER_TYPE_DELETE_STROKE;
	}
	else
	{
		cfg->eraserMode = ERASER_TYPE_NONE;
	}

	if (this->withDevice)
	{
		DeviceListHelper devList;
		std::vector<InputDevice>& devices = devList.getDeviceList();
		int dev = gtk_combo_box_get_active(GTK_COMBO_BOX(cbDevice)) - 1;

		if (dev < 0 || (int)devices.size() <= dev)
		{
			cfg->device = "";
		}
		else
		{
			cfg->device = devices[dev].getName();
		}

		cfg->disableDrawing = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cbDisableDrawing));
	}

	settings->customSettingsChanged();
}

void ButtonConfigGui::cbSelectCallback(GtkComboBox* widget, ButtonConfigGui* gui)
{
	XOJ_CHECK_TYPE_OBJ(gui, ButtonConfigGui);
	gui->enableDisableTools();
}

void ButtonConfigGui::enableDisableTools()
{
	XOJ_CHECK_TYPE(ButtonConfigGui);

	ToolType action = TOOL_NONE;
	GtkTreeIter iter;

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(cbTool), &iter);

	GValue value = {0};
	GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(cbTool));

	gtk_tree_model_get_value(model, &iter, 2, &value);
	action = (ToolType) g_value_get_int(&value);

	switch (action)
	{
	case TOOL_PEN:
	case TOOL_HILIGHTER:
		gtk_widget_set_sensitive(cbThickness, true);
		gtk_widget_set_sensitive(colorButton, true);
		gtk_widget_set_sensitive(cbDrawingType, true);
		gtk_widget_set_sensitive(cbEraserType, false);
		break;
		
	case TOOL_ERASER:
		gtk_widget_set_sensitive(cbThickness, true);
		gtk_widget_set_sensitive(colorButton, false);
		gtk_widget_set_sensitive(cbDrawingType, false);
		gtk_widget_set_sensitive(cbEraserType, true);
		break;
		
	case TOOL_TEXT:
		gtk_widget_set_sensitive(cbThickness, false);
		gtk_widget_set_sensitive(colorButton, true);
		gtk_widget_set_sensitive(cbDrawingType, false);
		gtk_widget_set_sensitive(cbEraserType, false);
		break;
		
	case TOOL_NONE:
	case TOOL_IMAGE:
		//case TOOL_DRAW_RECT:
		//case TOOL_DRAW_CIRCLE:
	case TOOL_SELECT_RECT:
	case TOOL_SELECT_REGION:
	case TOOL_VERTICAL_SPACE:
	case TOOL_HAND:
		gtk_widget_set_sensitive(cbThickness, false);
		gtk_widget_set_sensitive(colorButton, false);
		gtk_widget_set_sensitive(cbDrawingType, false);
		gtk_widget_set_sensitive(cbEraserType, false);
		break;
	default:
		break;
	}
}
