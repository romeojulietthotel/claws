/*
 * Sylpheed -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2007 Hiroyuki Yamamoto
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmenubar.h>
#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkitemfactory.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkutils.h>

#include "menu.h"
#include "utils.h"
#include "gtkutils.h"

#ifdef MAEMO
#ifdef CHINOOK
#include <hildon/hildon-program.h>
#else
#include <hildon-widgets/hildon-program.h>
#endif
#include <gtk/gtkmain.h>
#endif

static void connect_accel_change_signals(GtkWidget* widget, GtkWidget *wid2) ;


GtkWidget *menubar_create(GtkWidget *window, GtkItemFactoryEntry *entries,
			  guint n_entries, const gchar *path, gpointer data)
{
	GtkItemFactory *factory;
	GtkWidget *menubar;
	
#ifdef MAEMO
	factory = gtk_item_factory_new(GTK_TYPE_MENU, path, NULL);
#else
	factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, path, NULL);
#endif
	gtk_item_factory_set_translate_func(factory, menu_translate,
					    NULL, NULL);
	gtk_item_factory_create_items(factory, n_entries, entries, data);
	gtk_window_add_accel_group (GTK_WINDOW (window), factory->accel_group);

	menubar  = gtk_item_factory_get_widget(factory, path);
#ifdef MAEMO
	hildon_window_set_menu(HILDON_WINDOW(window), GTK_MENU(menubar));
#endif
	return menubar;
}

GtkWidget *menu_create_items(GtkItemFactoryEntry *entries,
			     guint n_entries, const gchar *path,
			     GtkItemFactory **factory, gpointer data)
{
	*factory = gtk_item_factory_new(GTK_TYPE_MENU, path, NULL);
	gtk_item_factory_set_translate_func(*factory, menu_translate,
					    NULL, NULL);
	gtk_item_factory_create_items(*factory, n_entries, entries, data);

	return gtk_item_factory_get_widget(*factory, path);
}

GtkActionGroup *cm_menu_create_action_group(const gchar *name, GtkActionEntry **entries,
					    gint num_entries, gpointer data)
{
	GtkActionGroup *group = gtk_action_group_new(name);
	gtk_action_group_set_translate_func(group, menu_translate, NULL, NULL);
	gtk_action_group_add_actions(group, entries, num_entries, data);
	gtk_ui_manager_insert_action_group(gtkut_ui_manager(), group, 0);
	return group;
}

gchar *menu_translate(const gchar *path, gpointer data)
{
	gchar *retval;

	retval = gettext(path);

	return retval;
}

void menu_set_sensitive(GtkItemFactory *ifactory, const gchar *path,
			gboolean sensitive)
{
	GtkWidget *widget;

	g_return_if_fail(ifactory != NULL);

	widget = gtk_item_factory_get_item(ifactory, path);
	g_return_if_fail(widget != NULL);

	gtk_widget_set_sensitive(widget, sensitive);
}

void cm_menu_set_sensitive(gchar *menu, gboolean sensitive)
{
	GtkUIManager *gui_manager = gtkut_ui_manager();
	GtkWidget *widget;
	gchar *path = g_strdup_printf("/Menus/%s/", menu);

	widget = gtk_ui_manager_get_widget(gui_manager, path);
	if( !GTK_IS_WIDGET(widget) ) {
		g_message("Blah, '%s' is not a widget.\n", path);
	}

	if( !GTK_IS_MENU_ITEM(widget) ) {
		g_message("Blah, '%s' is not a menu item.\n", path);
	}

	gtk_widget_set_sensitive(widget, sensitive);
	g_free(path);
}

void cm_toggle_menu_set_active(gchar *menu, gboolean active)
{
	GtkUIManager *gui_manager = gtkut_ui_manager();
	GtkWidget *widget;
	gchar *path = g_strdup_printf("/Menus/%s/", menu);

	widget = gtk_ui_manager_get_widget(gui_manager, path);
	if( !GTK_IS_WIDGET(widget) ) {
		g_message("Blah, '%s' is not a widget.\n", path);
	}

	if( !GTK_CHECK_MENU_ITEM(widget) ) {
		g_message("Blah, '%s' is not a check menu item.\n", path);
	}

	gtk_check_menu_item_set_active(widget, active);
	g_free(path);
}

void menu_set_sensitive_all(GtkMenuShell *menu_shell, gboolean sensitive)
{
	GList *cur;

	for (cur = menu_shell->children; cur != NULL; cur = cur->next)
		gtk_widget_set_sensitive(GTK_WIDGET(cur->data), sensitive);
}

void menu_set_active(GtkItemFactory *ifactory, const gchar *path,
		     gboolean is_active)
{
	GtkWidget *widget;

	g_return_if_fail(ifactory != NULL);

	widget = gtk_item_factory_get_item(ifactory, path);
	g_return_if_fail(widget != NULL);

	if (!GTK_IS_CHECK_MENU_ITEM(widget)) {
		debug_print("%s not check_menu_item\n", path?path:"(null)");
		return;
	}	
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(widget), is_active);
}

void menu_button_position(GtkMenu *menu, gint *x, gint *y, gboolean *push_in,
			  gpointer user_data)
{
        GtkWidget *widget;
        gint wheight;
        gint wx, wy;
	GtkRequisition mreq;
	GdkScreen *screen;
	GdkRectangle monitor;
	gint monitor_num;

	g_return_if_fail(x && y);
 	g_return_if_fail(GTK_IS_BUTTON(user_data));

	widget = GTK_WIDGET(user_data);

        gdk_window_get_origin(widget->window, x, y);
        wheight = widget->requisition.height;
        wx = widget->allocation.x;
        wy = widget->allocation.y;
        
	gtk_widget_size_request(GTK_WIDGET(menu), &mreq);
	screen = gtk_widget_get_screen (widget);
	monitor_num = gdk_screen_get_monitor_at_point (screen, *x, *y);
	gdk_screen_get_monitor_geometry (screen, monitor_num, 
					 &monitor);

        *x = *x + wx;
        *y = *y + wy + wheight;
	
	if (*y + mreq.height >= monitor.height)
		*y -= mreq.height;
}

gint menu_find_option_menu_index(GtkOptionMenu *optmenu, gpointer data,
				 GCompareFunc func)
{
	GtkWidget *menu;
	GtkWidget *menuitem;
	gpointer menu_data;
	GList *cur;
	gint n;

	menu = gtk_option_menu_get_menu(optmenu);

	for (cur = GTK_MENU_SHELL(menu)->children, n = 0;
	     cur != NULL; cur = cur->next, n++) {
		menuitem = GTK_WIDGET(cur->data);
		menu_data = g_object_get_data(G_OBJECT(menuitem),
					      MENU_VAL_ID);
		if (func) {
			if (func(menu_data, data) == 0)
				return n;
		} else if (menu_data == data)
			return n;
	}

	return -1;
}

static void connect_accel_change_signals(GtkWidget* widget, GtkWidget *wid2) 
{
#if 0
	g_signal_connect_after(G_OBJECT(widget), "add_accelerator", 
			       G_CALLBACK(menu_item_add_accel), wid2);
	g_signal_connect_after(G_OBJECT(widget), "remove_accelerator", 
			       G_CALLBACK(menu_item_remove_accel), wid2);
#endif
}

void menu_connect_identical_items(void)
{
	gint n;
	GtkWidget *item1;
	GtkWidget *item2;

	static const struct {	
		const gchar *path1;
		const gchar *path2;
	} pairs[] = {
		{"<Main>/Message/Reply",  			"<SummaryView>/Reply"},
#ifndef GENERIC_UMPC
		{"<Main>/Message/Reply to/all",  		"<SummaryView>/Reply to/all"},
		{"<Main>/Message/Reply to/sender", 		"<SummaryView>/Reply to/sender"},
		{"<Main>/Message/Reply to/mailing list",	"<SummaryView>/Reply to/mailing list"},
#endif
		{"<Main>/Message/Forward",		 	"<SummaryView>/Forward"},
#ifndef GENERIC_UMPC
		{"<Main>/Message/Redirect",			"<SummaryView>/Redirect"},
#endif
		{"<Main>/Message/Move...",			"<SummaryView>/Move..."},
		{"<Main>/Message/Copy...",			"<SummaryView>/Copy..."},
#ifndef GENERIC_UMPC
		{"<Main>/Message/Delete...",			"<SummaryView>/Delete..."},
#endif
		{"<Main>/Message/Mark/Mark",			"<SummaryView>/Mark/Mark"},
		{"<Main>/Message/Mark/Unmark",			"<SummaryView>/Mark/Unmark"},
		{"<Main>/Message/Mark/Mark as unread",		"<SummaryView>/Mark/Mark as unread"},
		{"<Main>/Message/Mark/Mark as read",		"<SummaryView>/Mark/Mark as read"},
		{"<Main>/Message/Mark/Mark all read",		"<SummaryView>/Mark/Mark all read"},
#ifndef GENERIC_UMPC
		{"<Main>/Tools/Add sender to address book",	"<SummaryView>/Add sender to address book"},
#endif
		{"<Main>/Tools/Create filter rule/Automatically",	
								"<SummaryView>/Create filter rule/Automatically"},
		{"<Main>/Tools/Create filter rule/by From",	"<SummaryView>/Create filter rule/by From"},
		{"<Main>/Tools/Create filter rule/by To",	"<SummaryView>/Create filter rule/by To"},
		{"<Main>/Tools/Create filter rule/by Subject",	"<SummaryView>/Create filter rule/by Subject"},
#ifndef GENERIC_UMPC
		{"<Main>/Tools/Create processing rule/Automatically",
								"<SummaryView>/Create processing rule/Automatically"},
		{"<Main>/Tools/Create processing rule/by From",	"<SummaryView>/Create processing rule/by From"},
		{"<Main>/Tools/Create processing rule/by To",	"<SummaryView>/Create processing rule/by To"},
		{"<Main>/Tools/Create processing rule/by Subject",
								"<SummaryView>/Create processing rule/by Subject"},
#endif
		{"<Main>/View/Open in new window",		"<SummaryView>/View/Open in new window"},
		{"<Main>/View/Message source",			"<SummaryView>/View/Message source"},
#ifndef GENERIC_UMPC
		{"<Main>/View/All headers",			"<SummaryView>/View/All headers"},
#endif
	};

	const gint numpairs = sizeof pairs / sizeof pairs[0];
	for (n = 0; n < numpairs; n++) {
		/* get widgets from the paths */

		item1 = gtk_item_factory_get_widget
				(gtk_item_factory_from_path(pairs[n].path1),pairs[n].path1);		
		item2 = gtk_item_factory_get_widget
				(gtk_item_factory_from_path(pairs[n].path2),pairs[n].path2);		

		if (item1 && item2) {
			/* connect widgets both ways around */
			connect_accel_change_signals(item2,item1);
			connect_accel_change_signals(item1,item2);
		} else { 
			if (!item1) debug_print(" ** Menu item not found: %s\n",pairs[n].path1);
			if (!item2) debug_print(" ** Menu item not found: %s\n",pairs[n].path2);
		}				
	}
}
