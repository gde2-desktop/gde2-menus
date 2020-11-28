/*
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GDE2MENU_TREE_H__
#define __GDE2MENU_TREE_H__

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Gde2MenuTree          Gde2MenuTree;
typedef struct Gde2MenuTreeItem      Gde2MenuTreeItem;
typedef struct Gde2MenuTreeDirectory Gde2MenuTreeDirectory;
typedef struct Gde2MenuTreeEntry     Gde2MenuTreeEntry;
typedef struct Gde2MenuTreeSeparator Gde2MenuTreeSeparator;
typedef struct Gde2MenuTreeHeader    Gde2MenuTreeHeader;
typedef struct Gde2MenuTreeAlias     Gde2MenuTreeAlias;

typedef void (*Gde2MenuTreeChangedFunc) (Gde2MenuTree* tree, gpointer user_data);

typedef enum {
	GDE2MENU_TREE_ITEM_INVALID = 0,
	GDE2MENU_TREE_ITEM_DIRECTORY,
	GDE2MENU_TREE_ITEM_ENTRY,
	GDE2MENU_TREE_ITEM_SEPARATOR,
	GDE2MENU_TREE_ITEM_HEADER,
	GDE2MENU_TREE_ITEM_ALIAS
} Gde2MenuTreeItemType;

#define GDE2MENU_TREE_ITEM(i)      ((Gde2MenuTreeItem*)(i))
#define GDE2MENU_TREE_DIRECTORY(i) ((Gde2MenuTreeDirectory*)(i))
#define GDE2MENU_TREE_ENTRY(i)     ((Gde2MenuTreeEntry*)(i))
#define GDE2MENU_TREE_SEPARATOR(i) ((Gde2MenuTreeSeparator*)(i))
#define GDE2MENU_TREE_HEADER(i)    ((Gde2MenuTreeHeader*)(i))
#define GDE2MENU_TREE_ALIAS(i)     ((Gde2MenuTreeAlias*)(i))

typedef enum {
	GDE2MENU_TREE_FLAGS_NONE                = 0,
	GDE2MENU_TREE_FLAGS_INCLUDE_EXCLUDED    = 1 << 0,
	GDE2MENU_TREE_FLAGS_SHOW_EMPTY          = 1 << 1,
	GDE2MENU_TREE_FLAGS_INCLUDE_NODISPLAY   = 1 << 2,
	GDE2MENU_TREE_FLAGS_SHOW_ALL_SEPARATORS = 1 << 3,
	GDE2MENU_TREE_FLAGS_MASK                = 0x0f
} Gde2MenuTreeFlags;

typedef enum {
	#define GDE2MENU_TREE_SORT_FIRST GDE2MENU_TREE_SORT_NAME
	GDE2MENU_TREE_SORT_NAME = 0,
	GDE2MENU_TREE_SORT_DISPLAY_NAME
	#define GDE2MENU_TREE_SORT_LAST GDE2MENU_TREE_SORT_DISPLAY_NAME
} Gde2MenuTreeSortKey;

Gde2MenuTree* gde2menu_tree_lookup(const char* menu_file, Gde2MenuTreeFlags flags);

Gde2MenuTree* gde2menu_tree_ref(Gde2MenuTree* tree);
void gde2menu_tree_unref(Gde2MenuTree* tree);

void gde2menu_tree_set_user_data(Gde2MenuTree* tree, gpointer user_data, GDestroyNotify dnotify);
gpointer gde2menu_tree_get_user_data(Gde2MenuTree* tree);

const char* gde2menu_tree_get_menu_file(Gde2MenuTree* tree);
Gde2MenuTreeDirectory* gde2menu_tree_get_root_directory(Gde2MenuTree* tree);
Gde2MenuTreeDirectory* gde2menu_tree_get_directory_from_path(Gde2MenuTree* tree, const char* path);

Gde2MenuTreeSortKey gde2menu_tree_get_sort_key(Gde2MenuTree* tree);
void gde2menu_tree_set_sort_key(Gde2MenuTree* tree, Gde2MenuTreeSortKey sort_key);



gpointer gde2menu_tree_item_ref(gpointer item);
void gde2menu_tree_item_unref(gpointer item);

void gde2menu_tree_item_set_user_data(Gde2MenuTreeItem* item, gpointer user_data, GDestroyNotify dnotify);
gpointer gde2menu_tree_item_get_user_data(Gde2MenuTreeItem* item);

Gde2MenuTreeItemType gde2menu_tree_item_get_type(Gde2MenuTreeItem* item);
Gde2MenuTreeDirectory* gde2menu_tree_item_get_parent(Gde2MenuTreeItem* item);


GSList* gde2menu_tree_directory_get_contents(Gde2MenuTreeDirectory* directory);
const char* gde2menu_tree_directory_get_name(Gde2MenuTreeDirectory* directory);
const char* gde2menu_tree_directory_get_comment(Gde2MenuTreeDirectory* directory);
const char* gde2menu_tree_directory_get_icon(Gde2MenuTreeDirectory* directory);
const char* gde2menu_tree_directory_get_desktop_file_path(Gde2MenuTreeDirectory* directory);
const char* gde2menu_tree_directory_get_menu_id(Gde2MenuTreeDirectory* directory);
Gde2MenuTree* gde2menu_tree_directory_get_tree(Gde2MenuTreeDirectory* directory);

gboolean gde2menu_tree_directory_get_is_nodisplay(Gde2MenuTreeDirectory* directory);

char* gde2menu_tree_directory_make_path(Gde2MenuTreeDirectory* directory, Gde2MenuTreeEntry* entry);


const char* gde2menu_tree_entry_get_name(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_generic_name(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_display_name(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_comment(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_icon(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_exec(Gde2MenuTreeEntry* entry);
gboolean gde2menu_tree_entry_get_launch_in_terminal(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_desktop_file_path(Gde2MenuTreeEntry* entry);
const char* gde2menu_tree_entry_get_desktop_file_id(Gde2MenuTreeEntry* entry);
gboolean gde2menu_tree_entry_get_is_excluded(Gde2MenuTreeEntry* entry);
gboolean gde2menu_tree_entry_get_is_nodisplay(Gde2MenuTreeEntry* entry);

Gde2MenuTreeDirectory* gde2menu_tree_header_get_directory(Gde2MenuTreeHeader* header);

Gde2MenuTreeDirectory* gde2menu_tree_alias_get_directory(Gde2MenuTreeAlias* alias);
Gde2MenuTreeItem* gde2menu_tree_alias_get_item(Gde2MenuTreeAlias* alias);

void gde2menu_tree_add_monitor(Gde2MenuTree* tree, Gde2MenuTreeChangedFunc callback, gpointer user_data);
void gde2menu_tree_remove_monitor(Gde2MenuTree* tree, Gde2MenuTreeChangedFunc callback, gpointer user_data);

#ifdef __cplusplus
}
#endif

#endif /* __GDE2MENU_TREE_H__ */
