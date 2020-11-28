/*
 * Copyright (C) 2005 Red Hat, Inc.
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
 * Boston, MA  02110-1301, USA.
 */

#include <config.h>

#include <Python.h>
#include <gde2menu-tree.h>

typedef struct {
	PyObject_HEAD
	Gde2MenuTree* tree;
	GSList* callbacks;
} PyGde2MenuTree;

typedef struct {
	PyObject* tree;
	PyObject* callback;
	PyObject* user_data;
} PyGde2MenuTreeCallback;

typedef struct {
	PyObject_HEAD
	Gde2MenuTreeItem* item;
} PyGde2MenuTreeItem;

typedef PyGde2MenuTreeItem PyGde2MenuTreeDirectory;
typedef PyGde2MenuTreeItem PyGde2MenuTreeEntry;
typedef PyGde2MenuTreeItem PyGde2MenuTreeSeparator;
typedef PyGde2MenuTreeItem PyGde2MenuTreeHeader;
typedef PyGde2MenuTreeItem PyGde2MenuTreeAlias;

static PyGde2MenuTree* pygde2menu_tree_wrap(Gde2MenuTree* tree);
static PyGde2MenuTreeDirectory* pygde2menu_tree_directory_wrap(Gde2MenuTreeDirectory* directory);
static PyGde2MenuTreeEntry* pygde2menu_tree_entry_wrap(Gde2MenuTreeEntry* entry);
static PyGde2MenuTreeSeparator* pygde2menu_tree_separator_wrap(Gde2MenuTreeSeparator* separator);
static PyGde2MenuTreeHeader* pygde2menu_tree_header_wrap(Gde2MenuTreeHeader* header);
static PyGde2MenuTreeAlias* pygde2menu_tree_alias_wrap(Gde2MenuTreeAlias* alias);

static inline PyObject* lookup_item_type_str(const char* item_type_str)
{
	PyObject* module;

	module = PyDict_GetItemString(PyImport_GetModuleDict(), "gde2menu");

	return PyDict_GetItemString(PyModule_GetDict(module), item_type_str);
}

static void pygde2menu_tree_item_dealloc(PyGde2MenuTreeItem* self)
{
	if (self->item != NULL)
	{
		gde2menu_tree_item_set_user_data(self->item, NULL, NULL);
		gde2menu_tree_item_unref(self->item);
		self->item = NULL;
	}

	PyObject_DEL (self);
}

static PyObject* pygde2menu_tree_item_get_type(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeItem* item;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Item.get_type"))
		{
			return NULL;
		}
	}

	item = (PyGde2MenuTreeItem*) self;

	switch (gde2menu_tree_item_get_type(item->item))
	{
		case GDE2MENU_TREE_ITEM_DIRECTORY:
			retval = lookup_item_type_str("TYPE_DIRECTORY");
			break;

		case GDE2MENU_TREE_ITEM_ENTRY:
			retval = lookup_item_type_str("TYPE_ENTRY");
			break;

		case GDE2MENU_TREE_ITEM_SEPARATOR:
			retval = lookup_item_type_str("TYPE_SEPARATOR");
			break;

		case GDE2MENU_TREE_ITEM_HEADER:
			retval = lookup_item_type_str("TYPE_HEADER");
			break;

		case GDE2MENU_TREE_ITEM_ALIAS:
			retval = lookup_item_type_str("TYPE_ALIAS");
			break;

		default:
			g_assert_not_reached();
			break;
	}

	Py_INCREF(retval);

	return retval;
}

static PyObject* pygde2menu_tree_item_get_parent(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeItem* item;
	Gde2MenuTreeDirectory* parent;
	PyGde2MenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Item.get_parent"))
		{
			return NULL;
		}
	}

	item = (PyGde2MenuTreeItem*) self;

	parent = gde2menu_tree_item_get_parent(item->item);

	if (parent == NULL)
	{
		Py_INCREF(Py_None);

		return Py_None;
	}

	retval = pygde2menu_tree_directory_wrap(parent);

	gde2menu_tree_item_unref(parent);

	return (PyObject*) retval;
}

static struct PyMethodDef pygde2menu_tree_item_methods[] = {
	{"get_type", pygde2menu_tree_item_get_type,   METH_VARARGS},
	{"get_parent", pygde2menu_tree_item_get_parent, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyGde2MenuTreeItem_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                             /* ob_size */
	"gde2menu.Item",                               /* tp_name */
	sizeof(PyGde2MenuTreeItem),                    /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pygde2menu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(cmpfunc) 0,                                   /* tp_compare */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) 0,                              /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,      /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pygde2menu_tree_item_methods,                  /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
	0,                                             /* tp_free */
	(inquiry) 0,                                   /* tp_is_gc */
	(PyObject*) 0,                                 /* tp_bases */
};

static PyObject* pygde2menu_tree_directory_get_contents(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	PyObject* retval;
	GSList* contents;
	GSList* tmp;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Directory.get_contents"))
		{
			return NULL;
		}
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	retval = PyList_New(0);

	contents = gde2menu_tree_directory_get_contents(GDE2MENU_TREE_DIRECTORY(directory->item));

	tmp = contents;

	while (tmp != NULL)
	{
		Gde2MenuTreeItem* item = tmp->data;
		PyObject* pyitem;

		switch (gde2menu_tree_item_get_type(item))
		{
			case GDE2MENU_TREE_ITEM_DIRECTORY:
				pyitem = (PyObject*) pygde2menu_tree_directory_wrap(GDE2MENU_TREE_DIRECTORY(item));
				break;

			case GDE2MENU_TREE_ITEM_ENTRY:
				pyitem = (PyObject*) pygde2menu_tree_entry_wrap(GDE2MENU_TREE_ENTRY(item));
				break;

			case GDE2MENU_TREE_ITEM_SEPARATOR:
				pyitem = (PyObject*) pygde2menu_tree_separator_wrap(GDE2MENU_TREE_SEPARATOR(item));
				break;

			case GDE2MENU_TREE_ITEM_HEADER:
				pyitem = (PyObject*) pygde2menu_tree_header_wrap(GDE2MENU_TREE_HEADER(item));
				break;

			case GDE2MENU_TREE_ITEM_ALIAS:
				pyitem = (PyObject*) pygde2menu_tree_alias_wrap(GDE2MENU_TREE_ALIAS(item));
				break;

			default:
				g_assert_not_reached();
				break;
		}

		PyList_Append(retval, pyitem);
		Py_DECREF(pyitem);

		gde2menu_tree_item_unref(item);

		tmp = tmp->next;
	}

	g_slist_free(contents);

	return retval;
}

static PyObject* pygde2menu_tree_directory_get_name(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	const char* name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Directory.get_name"))
		{
			return NULL;
		}
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	name = gde2menu_tree_directory_get_name(GDE2MENU_TREE_DIRECTORY(directory->item));

	if (name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(name);
}

static PyObject* pygde2menu_tree_directory_get_comment(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	const char* comment;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Directory.get_comment"))
		{
			return NULL;
		}
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	comment = gde2menu_tree_directory_get_comment(GDE2MENU_TREE_DIRECTORY(directory->item));

	if (comment == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(comment);
}

static PyObject* pygde2menu_tree_directory_get_icon(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	const char* icon;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Directory.get_icon"))
		{
			return NULL;
		}
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	icon = gde2menu_tree_directory_get_icon(GDE2MENU_TREE_DIRECTORY(directory->item));

	if (icon == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
    }

	return PyString_FromString(icon);
}

static PyObject* pygde2menu_tree_directory_get_desktop_file_path(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	const char* path;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Directory.get_desktop_file_path"))
		{
			return NULL;
		}
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	path = gde2menu_tree_directory_get_desktop_file_path(GDE2MENU_TREE_DIRECTORY(directory->item));

	if (path == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(path);
}

static PyObject* pygde2menu_tree_directory_get_menu_id(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	const char* menu_id;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Directory.get_menu_id"))
		{
			return NULL;
		}
    }

	directory = (PyGde2MenuTreeDirectory*) self;

	menu_id = gde2menu_tree_directory_get_menu_id(GDE2MENU_TREE_DIRECTORY(directory->item));

	if (menu_id == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(menu_id);
}

static PyObject* pygde2menu_tree_directory_get_tree(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	Gde2MenuTree* tree;
	PyGde2MenuTree* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Item.get_tree"))
		{
			return NULL;
		}
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	tree = gde2menu_tree_directory_get_tree(GDE2MENU_TREE_DIRECTORY(directory->item));

	if (tree == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pygde2menu_tree_wrap(tree);

	gde2menu_tree_unref(tree);

	return (PyObject*) retval;
}

static PyObject* pygde2menu_tree_directory_make_path(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeDirectory* directory;
	PyGde2MenuTreeEntry* entry;
	PyObject* retval;
	char* path;

	if (!PyArg_ParseTuple(args, "O:gde2menu.Directory.make_path", &entry))
	{
		return NULL;
	}

	directory = (PyGde2MenuTreeDirectory*) self;

	path = gde2menu_tree_directory_make_path(GDE2MENU_TREE_DIRECTORY(directory->item), GDE2MENU_TREE_ENTRY(entry->item));

	if (path == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = PyString_FromString(path);

	g_free(path);

	return retval;
}

static PyObject* pygde2menu_tree_directory_getattro(PyGde2MenuTreeDirectory* self, PyObject* py_attr)
{
	if (PyString_Check(py_attr))
	{
		char* attr;

		attr = PyString_AsString(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sssssssss]",
				"type",
				"parent",
				"contents",
				"name",
				"comment",
				"icon",
				"desktop_file_path",
				"menu_id",
				"tree");
		}
		else if (!strcmp(attr, "type"))
		{
			return pygde2menu_tree_item_get_type((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "parent"))
		{
			return pygde2menu_tree_item_get_parent((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "contents"))
		{
			return pygde2menu_tree_directory_get_contents((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "name"))
		{
			return pygde2menu_tree_directory_get_name((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "comment"))
		{
			return pygde2menu_tree_directory_get_comment((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "icon"))
		{
			return pygde2menu_tree_directory_get_icon((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "desktop_file_path"))
		{
			return pygde2menu_tree_directory_get_desktop_file_path((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "menu_id"))
		{
			return pygde2menu_tree_directory_get_menu_id((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "tree"))
		{
			return pygde2menu_tree_directory_get_tree((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pygde2menu_tree_directory_methods[] = {
	{"get_contents", pygde2menu_tree_directory_get_contents, METH_VARARGS},
	{"get_name", pygde2menu_tree_directory_get_name, METH_VARARGS},
	{"get_comment", pygde2menu_tree_directory_get_comment, METH_VARARGS},
	{"get_icon", pygde2menu_tree_directory_get_icon, METH_VARARGS},
	{"get_desktop_file_path", pygde2menu_tree_directory_get_desktop_file_path, METH_VARARGS},
	{"get_menu_id", pygde2menu_tree_directory_get_menu_id, METH_VARARGS},
	{"get_tree", pygde2menu_tree_directory_get_tree, METH_VARARGS},
	{"make_path", pygde2menu_tree_directory_make_path, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyGde2MenuTreeDirectory_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                              /* ob_size */
	"gde2menu.Directory",                           /* tp_name */
	sizeof(PyGde2MenuTreeDirectory),                /* tp_basicsize */
	0,                                              /* tp_itemsize */
	(destructor) pygde2menu_tree_item_dealloc,      /* tp_dealloc */
	(printfunc) 0,                                  /* tp_print */
	(getattrfunc) 0,                                /* tp_getattr */
	(setattrfunc) 0,                                /* tp_setattr */
	(cmpfunc) 0,                                    /* tp_compare */
	(reprfunc) 0,                                   /* tp_repr */
	0,                                              /* tp_as_number */
	0,                                              /* tp_as_sequence */
	0,                                              /* tp_as_mapping */
	(hashfunc) 0,                                   /* tp_hash */
	(ternaryfunc) 0,                                /* tp_call */
	(reprfunc) 0,                                   /* tp_str */
	(getattrofunc) pygde2menu_tree_directory_getattro, /* tp_getattro */
	(setattrofunc) 0,                               /* tp_setattro */
	0,                                              /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                             /* tp_flags */
	NULL,                                           /* Documentation string */
	(traverseproc) 0,                               /* tp_traverse */
	(inquiry) 0,                                    /* tp_clear */
	(richcmpfunc) 0,                                /* tp_richcompare */
	0,                                              /* tp_weaklistoffset */
	(getiterfunc) 0,                                /* tp_iter */
	(iternextfunc) 0,                               /* tp_iternext */
	pygde2menu_tree_directory_methods,              /* tp_methods */
	0,                                              /* tp_members */
	0,                                              /* tp_getset */
	(PyTypeObject*) 0,                              /* tp_base */
	(PyObject*) 0,                                  /* tp_dict */
	0,                                              /* tp_descr_get */
	0,                                              /* tp_descr_set */
	0,                                              /* tp_dictoffset */
	(initproc) 0,                                   /* tp_init */
	0,                                              /* tp_alloc */
	0,                                              /* tp_new */
	0,                                              /* tp_free */
	(inquiry) 0,                                    /* tp_is_gc */
	(PyObject*) 0,                                  /* tp_bases */
};

static PyGde2MenuTreeDirectory* pygde2menu_tree_directory_wrap(Gde2MenuTreeDirectory* directory)
{
	PyGde2MenuTreeDirectory* retval;

	if ((retval = gde2menu_tree_item_get_user_data(GDE2MENU_TREE_ITEM(directory))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyGde2MenuTreeDirectory*) PyObject_NEW(PyGde2MenuTreeDirectory, &PyGde2MenuTreeDirectory_Type)))
	{
		return NULL;
	}

	retval->item = gde2menu_tree_item_ref(directory);

	gde2menu_tree_item_set_user_data(GDE2MENU_TREE_ITEM(directory), retval, NULL);

	return retval;
}

static PyObject* pygde2menu_tree_entry_get_name(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_name"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	name = gde2menu_tree_entry_get_name(GDE2MENU_TREE_ENTRY(entry->item));

	if (name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(name);
}

static PyObject* pygde2menu_tree_entry_get_generic_name(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* generic_name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_generic_name"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	generic_name = gde2menu_tree_entry_get_generic_name(GDE2MENU_TREE_ENTRY(entry->item));

	if (generic_name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(generic_name);
}

static PyObject* pygde2menu_tree_entry_get_display_name(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* display_name;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_display_name"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	display_name = gde2menu_tree_entry_get_display_name(GDE2MENU_TREE_ENTRY(entry->item));

	if (display_name == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(display_name);
}

static PyObject* pygde2menu_tree_entry_get_comment(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* comment;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_comment"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	comment = gde2menu_tree_entry_get_comment(GDE2MENU_TREE_ENTRY(entry->item));

	if (comment == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(comment);
}

static PyObject* pygde2menu_tree_entry_get_icon(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* icon;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_icon"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	icon = gde2menu_tree_entry_get_icon(GDE2MENU_TREE_ENTRY(entry->item));

	if (icon == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(icon);
}

static PyObject* pygde2menu_tree_entry_get_exec(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* exec;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_exec"))
		{
			return NULL;
		}
    }

	entry = (PyGde2MenuTreeEntry*) self;

	exec = gde2menu_tree_entry_get_exec(GDE2MENU_TREE_ENTRY(entry->item));

	if (exec == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(exec);
}

static PyObject* pygde2menu_tree_entry_get_launch_in_terminal(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_launch_in_terminal"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	if (gde2menu_tree_entry_get_launch_in_terminal(GDE2MENU_TREE_ENTRY(entry->item)))
	{
		retval = Py_True;
	}
	else
	{
		retval = Py_False;
	}

	Py_INCREF(retval);

	return retval;
}

static PyObject* pygde2menu_tree_entry_get_desktop_file_path(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* desktop_file_path;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_desktop_file_path"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	desktop_file_path = gde2menu_tree_entry_get_desktop_file_path(GDE2MENU_TREE_ENTRY(entry->item));

	if (desktop_file_path == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(desktop_file_path);
}

static PyObject* pygde2menu_tree_entry_get_desktop_file_id(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	const char* desktop_file_id;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_desktop_file_id"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	desktop_file_id = gde2menu_tree_entry_get_desktop_file_id(GDE2MENU_TREE_ENTRY(entry->item));

	if (desktop_file_id == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(desktop_file_id);
}

static PyObject* pygde2menu_tree_entry_get_is_excluded(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_is_excluded"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	retval = gde2menu_tree_entry_get_is_excluded(GDE2MENU_TREE_ENTRY(entry->item)) ? Py_True : Py_False;
	Py_INCREF(retval);

	return retval;
}

static PyObject* pygde2menu_tree_entry_get_is_nodisplay(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeEntry* entry;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Entry.get_is_nodisplay"))
		{
			return NULL;
		}
	}

	entry = (PyGde2MenuTreeEntry*) self;

	if (gde2menu_tree_entry_get_is_nodisplay(GDE2MENU_TREE_ENTRY(entry->item)))
	{
		retval = Py_True;
	}
	else
	{
		retval = Py_False;
	}

	Py_INCREF(retval);

	return retval;
}

static PyObject* pygde2menu_tree_entry_getattro(PyGde2MenuTreeEntry* self, PyObject* py_attr)
{
	if (PyString_Check(py_attr))
	{
		char* attr;

		attr = PyString_AsString(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sssssssssss]",
				"type",
				"parent",
				"name",
				"comment",
				"icon",
				"exec_info",
				"launch_in_terminal",
				"desktop_file_path",
				"desktop_file_id",
				"is_excluded",
				"is_nodisplay");
		}
		else if (!strcmp(attr, "type"))
		{
			return pygde2menu_tree_item_get_type((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "parent"))
		{
			return pygde2menu_tree_item_get_parent((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "name"))
		{
			return pygde2menu_tree_entry_get_name((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "generic_name"))
		{
			return pygde2menu_tree_entry_get_generic_name((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "display_name"))
		{
			return pygde2menu_tree_entry_get_display_name((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "comment"))
		{
			return pygde2menu_tree_entry_get_comment((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "icon"))
		{
			return pygde2menu_tree_entry_get_icon((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "exec_info"))
		{
			return pygde2menu_tree_entry_get_exec((PyObject*) self, NULL);
		}
			else if (!strcmp(attr, "launch_in_terminal"))
		{
			return pygde2menu_tree_entry_get_launch_in_terminal((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "desktop_file_path"))
		{
			return pygde2menu_tree_entry_get_desktop_file_path((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "desktop_file_id"))
		{
			return pygde2menu_tree_entry_get_desktop_file_id((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "is_excluded"))
		{
			return pygde2menu_tree_entry_get_is_excluded((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "is_nodisplay"))
		{
			return pygde2menu_tree_entry_get_is_nodisplay((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pygde2menu_tree_entry_methods[] = {
	{"get_name", pygde2menu_tree_entry_get_name, METH_VARARGS},
	{"get_generic_name", pygde2menu_tree_entry_get_generic_name, METH_VARARGS},
	{"get_display_name", pygde2menu_tree_entry_get_display_name, METH_VARARGS},
	{"get_comment", pygde2menu_tree_entry_get_comment, METH_VARARGS},
	{"get_icon", pygde2menu_tree_entry_get_icon, METH_VARARGS},
	{"get_exec", pygde2menu_tree_entry_get_exec, METH_VARARGS},
	{"get_launch_in_terminal", pygde2menu_tree_entry_get_launch_in_terminal, METH_VARARGS},
	{"get_desktop_file_path", pygde2menu_tree_entry_get_desktop_file_path, METH_VARARGS},
	{"get_desktop_file_id", pygde2menu_tree_entry_get_desktop_file_id, METH_VARARGS},
	{"get_is_excluded", pygde2menu_tree_entry_get_is_excluded, METH_VARARGS},
	{"get_is_nodisplay", pygde2menu_tree_entry_get_is_nodisplay, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyGde2MenuTreeEntry_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                             /* ob_size */
	"gde2menu.Entry",                              /* tp_name */
	sizeof(PyGde2MenuTreeEntry),                   /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pygde2menu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(cmpfunc) 0,                                   /* tp_compare */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) pygde2menu_tree_entry_getattro, /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pygde2menu_tree_entry_methods,                 /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
	0,                                             /* tp_free */
	(inquiry) 0,                                   /* tp_is_gc */
	(PyObject*) 0,                                 /* tp_bases */
};

static PyGde2MenuTreeEntry* pygde2menu_tree_entry_wrap(Gde2MenuTreeEntry* entry)
{
	PyGde2MenuTreeEntry* retval;

	if ((retval = gde2menu_tree_item_get_user_data(GDE2MENU_TREE_ITEM(entry))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyGde2MenuTreeEntry*) PyObject_NEW(PyGde2MenuTreeEntry, &PyGde2MenuTreeEntry_Type)))
	{
		return NULL;
	}

	retval->item = gde2menu_tree_item_ref(entry);

	gde2menu_tree_item_set_user_data(GDE2MENU_TREE_ITEM(entry), retval, NULL);

	return retval;
}

static PyTypeObject PyGde2MenuTreeSeparator_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                             /* ob_size */
	"gde2menu.Separator",                          /* tp_name */
	sizeof(PyGde2MenuTreeSeparator),               /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pygde2menu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(cmpfunc) 0,                                   /* tp_compare */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) 0,                              /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	NULL,                                          /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
	0,                                             /* tp_free */
	(inquiry) 0,                                   /* tp_is_gc */
	(PyObject*) 0,                                 /* tp_bases */
};

static PyGde2MenuTreeSeparator* pygde2menu_tree_separator_wrap(Gde2MenuTreeSeparator* separator)
{
	PyGde2MenuTreeSeparator* retval;

	if ((retval = gde2menu_tree_item_get_user_data(GDE2MENU_TREE_ITEM(separator))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyGde2MenuTreeSeparator*) PyObject_NEW(PyGde2MenuTreeSeparator, &PyGde2MenuTreeSeparator_Type)))
	{
		return NULL;
	}

	retval->item = gde2menu_tree_item_ref(separator);

	gde2menu_tree_item_set_user_data(GDE2MENU_TREE_ITEM(separator), retval, NULL);

	return retval;
}

static PyObject* pygde2menu_tree_header_get_directory(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeHeader* header;
	Gde2MenuTreeDirectory* directory;
	PyGde2MenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Header.get_directory"))
		{
			return NULL;
		}
	}

	header = (PyGde2MenuTreeHeader*) self;

	directory = gde2menu_tree_header_get_directory(GDE2MENU_TREE_HEADER(header->item));

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
    }

	retval = pygde2menu_tree_directory_wrap(directory);

	gde2menu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pygde2menu_tree_header_getattro(PyGde2MenuTreeHeader* self, PyObject* py_attr)
{
	if (PyString_Check(py_attr))
	{
		char* attr;

		attr = PyString_AsString(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sss]",
				"type",
				"parent",
				"directory");
		}
		else if (!strcmp(attr, "type"))
		{
			return pygde2menu_tree_item_get_type((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "parent"))
		{
			return pygde2menu_tree_item_get_parent((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "directory"))
		{
			return pygde2menu_tree_header_get_directory((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pygde2menu_tree_header_methods[] = {
	{"get_directory", pygde2menu_tree_header_get_directory, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyGde2MenuTreeHeader_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                             /* ob_size */
	"gde2menu.Header",                             /* tp_name */
	sizeof(PyGde2MenuTreeHeader),                  /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pygde2menu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(cmpfunc) 0,                                   /* tp_compare */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) pygde2menu_tree_header_getattro, /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pygde2menu_tree_header_methods,                /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
	0,                                             /* tp_free */
	(inquiry) 0,                                   /* tp_is_gc */
	(PyObject*) 0,                                 /* tp_bases */
};

static PyGde2MenuTreeHeader* pygde2menu_tree_header_wrap(Gde2MenuTreeHeader* header)
{
	PyGde2MenuTreeHeader* retval;

	if ((retval = gde2menu_tree_item_get_user_data(GDE2MENU_TREE_ITEM(header))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyGde2MenuTreeHeader*) PyObject_NEW(PyGde2MenuTreeHeader, &PyGde2MenuTreeHeader_Type)))
	{
		return NULL;
	}

	retval->item = gde2menu_tree_item_ref(header);

	gde2menu_tree_item_set_user_data(GDE2MENU_TREE_ITEM(header), retval, NULL);

	return retval;
}

static PyObject* pygde2menu_tree_alias_get_directory(PyObject*self, PyObject* args)
{
	PyGde2MenuTreeAlias* alias;
	Gde2MenuTreeDirectory* directory;
	PyGde2MenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Alias.get_directory"))
		{
			return NULL;
		}
	}

	alias = (PyGde2MenuTreeAlias*) self;

	directory = gde2menu_tree_alias_get_directory(GDE2MENU_TREE_ALIAS(alias->item));

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pygde2menu_tree_directory_wrap(directory);

	gde2menu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pygde2menu_tree_alias_get_item(PyObject* self, PyObject* args)
{
	PyGde2MenuTreeAlias* alias;
	Gde2MenuTreeItem* item;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Alias.get_item"))
		{
			return NULL;
		}
	}

	alias = (PyGde2MenuTreeAlias*) self;

	item = gde2menu_tree_alias_get_item(GDE2MENU_TREE_ALIAS(alias->item));

	if (item == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	switch (gde2menu_tree_item_get_type(item))
	{
		case GDE2MENU_TREE_ITEM_DIRECTORY:
			retval = (PyObject*) pygde2menu_tree_directory_wrap(GDE2MENU_TREE_DIRECTORY(item));
			break;

		case GDE2MENU_TREE_ITEM_ENTRY:
			retval = (PyObject*) pygde2menu_tree_entry_wrap(GDE2MENU_TREE_ENTRY(item));
			break;

		default:
			g_assert_not_reached();
			break;
	}

	gde2menu_tree_item_unref(item);

	return retval;
}

static PyObject* pygde2menu_tree_alias_getattro(PyGde2MenuTreeAlias* self, PyObject* py_attr)
{
	if (PyString_Check(py_attr))
	{
		char* attr;

		attr = PyString_AsString(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[ssss]",
				"type",
				"parent",
				"directory",
				"item");
		}
		else if (!strcmp(attr, "type"))
		{
			return pygde2menu_tree_item_get_type((PyObject*) self, NULL);
		}
		  else if (!strcmp(attr, "parent"))
		{
			return pygde2menu_tree_item_get_parent((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "directory"))
		{
			return pygde2menu_tree_alias_get_directory((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "item"))
		{
			return pygde2menu_tree_alias_get_item((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static struct PyMethodDef pygde2menu_tree_alias_methods[] = {
	{"get_directory", pygde2menu_tree_alias_get_directory, METH_VARARGS},
	{"get_item", pygde2menu_tree_alias_get_item, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyGde2MenuTreeAlias_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                             /* ob_size */
	"gde2menu.Alias",                              /* tp_name */
	sizeof(PyGde2MenuTreeAlias),                   /* tp_basicsize */
	0,                                             /* tp_itemsize */
	(destructor) pygde2menu_tree_item_dealloc,     /* tp_dealloc */
	(printfunc) 0,                                 /* tp_print */
	(getattrfunc) 0,                               /* tp_getattr */
	(setattrfunc) 0,                               /* tp_setattr */
	(cmpfunc) 0,                                   /* tp_compare */
	(reprfunc) 0,                                  /* tp_repr */
	0,                                             /* tp_as_number */
	0,                                             /* tp_as_sequence */
	0,                                             /* tp_as_mapping */
	(hashfunc) 0,                                  /* tp_hash */
	(ternaryfunc) 0,                               /* tp_call */
	(reprfunc) 0,                                  /* tp_str */
	(getattrofunc) pygde2menu_tree_alias_getattro, /* tp_getattro */
	(setattrofunc) 0,                              /* tp_setattro */
	0,                                             /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                            /* tp_flags */
	NULL,                                          /* Documentation string */
	(traverseproc) 0,                              /* tp_traverse */
	(inquiry) 0,                                   /* tp_clear */
	(richcmpfunc) 0,                               /* tp_richcompare */
	0,                                             /* tp_weaklistoffset */
	(getiterfunc) 0,                               /* tp_iter */
	(iternextfunc) 0,                              /* tp_iternext */
	pygde2menu_tree_alias_methods,                 /* tp_methods */
	0,                                             /* tp_members */
	0,                                             /* tp_getset */
	(PyTypeObject*) 0,                             /* tp_base */
	(PyObject*) 0,                                 /* tp_dict */
	0,                                             /* tp_descr_get */
	0,                                             /* tp_descr_set */
	0,                                             /* tp_dictoffset */
	(initproc) 0,                                  /* tp_init */
	0,                                             /* tp_alloc */
	0,                                             /* tp_new */
	0,                                             /* tp_free */
	(inquiry) 0,                                   /* tp_is_gc */
	(PyObject*) 0,                                 /* tp_bases */
};

static PyGde2MenuTreeAlias* pygde2menu_tree_alias_wrap(Gde2MenuTreeAlias* alias)
{
	PyGde2MenuTreeAlias* retval;

	if ((retval = gde2menu_tree_item_get_user_data(GDE2MENU_TREE_ITEM(alias))) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyGde2MenuTreeAlias*) PyObject_NEW(PyGde2MenuTreeAlias, &PyGde2MenuTreeAlias_Type)))
	{
		return NULL;
	}

	retval->item = gde2menu_tree_item_ref(alias);

	gde2menu_tree_item_set_user_data(GDE2MENU_TREE_ITEM(alias), retval, NULL);

	return retval;
}

static PyObject* pygde2menu_tree_get_menu_file(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	const char* menu_file;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Tree.get_menu_file"))
		{
			return NULL;
		}
	}

	tree = (PyGde2MenuTree*) self;

	menu_file = gde2menu_tree_get_menu_file(tree->tree);

	if (menu_file == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	return PyString_FromString(menu_file);
}

static PyObject* pygde2menu_tree_get_root_directory(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	Gde2MenuTreeDirectory* directory;
	PyGde2MenuTreeDirectory* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Tree.get_root_directory"))
		{
			return NULL;
		}
	}

	tree = (PyGde2MenuTree*) self;

	directory = gde2menu_tree_get_root_directory(tree->tree);

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pygde2menu_tree_directory_wrap (directory);

	gde2menu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pygde2menu_tree_get_directory_from_path(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	Gde2MenuTreeDirectory* directory;
	PyGde2MenuTreeDirectory* retval;
	char* path;

	if (!PyArg_ParseTuple(args, "s:gde2menu.Tree.get_directory_from_path", &path))
	{
		return NULL;
	}

	tree = (PyGde2MenuTree*) self;

	directory = gde2menu_tree_get_directory_from_path(tree->tree, path);

	if (directory == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pygde2menu_tree_directory_wrap(directory);

	gde2menu_tree_item_unref(directory);

	return (PyObject*) retval;
}

static PyObject* pygde2menu_tree_get_sort_key(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	PyObject* retval;

	if (args != NULL)
	{
		if (!PyArg_ParseTuple(args, ":gde2menu.Tree.get_sort_key"))
		{
			return NULL;
		}
	}

	tree = (PyGde2MenuTree*) self;

	switch (gde2menu_tree_get_sort_key(tree->tree))
	{
		case GDE2MENU_TREE_SORT_NAME:
			retval = lookup_item_type_str("SORT_NAME");
			break;

		case GDE2MENU_TREE_SORT_DISPLAY_NAME:
			retval = lookup_item_type_str("SORT_DISPLAY_NAME");
			break;

		default:
			g_assert_not_reached();
			break;
	}

	return (PyObject*) retval;
}

static PyObject* pygde2menu_tree_set_sort_key(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	int sort_key;

	if (!PyArg_ParseTuple(args, "i:gde2menu.Tree.set_sort_key", &sort_key))
	{
		return NULL;
	}

	tree = (PyGde2MenuTree*) self;

	gde2menu_tree_set_sort_key(tree->tree, sort_key);

	return Py_None;
}

static PyGde2MenuTreeCallback* pygde2menu_tree_callback_new(PyObject* tree, PyObject* callback, PyObject* user_data)
{
	PyGde2MenuTreeCallback* retval;

	retval = g_new0(PyGde2MenuTreeCallback, 1);

	Py_INCREF(tree);
	retval->tree = tree;

	Py_INCREF(callback);
	retval->callback = callback;

	Py_XINCREF(user_data);
	retval->user_data = user_data;

	return retval;
}

static void pygde2menu_tree_callback_free(PyGde2MenuTreeCallback* callback)
{
	Py_XDECREF(callback->user_data);
	callback->user_data = NULL;

	Py_DECREF(callback->callback);
	callback->callback = NULL;

	Py_DECREF(callback->tree);
	callback->tree = NULL;

	g_free(callback);
}

static void pygde2menu_tree_handle_monitor_callback(Gde2MenuTree* tree, PyGde2MenuTreeCallback* callback)
{
	PyObject* args;
	PyObject* ret;
	PyGILState_STATE gstate;

	gstate = PyGILState_Ensure();

	args = PyTuple_New(callback->user_data ? 2 : 1);

	Py_INCREF(callback->tree);
	PyTuple_SET_ITEM(args, 0, callback->tree);

	if (callback->user_data != NULL)
	{
		Py_INCREF(callback->user_data);
		PyTuple_SET_ITEM(args, 1, callback->user_data);
	}

	ret = PyObject_CallObject(callback->callback, args);

	Py_XDECREF(ret);
	Py_DECREF(args);

	PyGILState_Release(gstate);
}

static PyObject* pygde2menu_tree_add_monitor(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	PyGde2MenuTreeCallback* callback;
	PyObject* pycallback;
	PyObject* pyuser_data = NULL;

	if (!PyArg_ParseTuple(args, "O|O:gde2menu.Tree.add_monitor", &pycallback, &pyuser_data))
	{
		return NULL;
	}

	if (!PyCallable_Check(pycallback))
	{
		PyErr_SetString(PyExc_TypeError, "callback must be callable");
		return NULL;
	}

	tree = (PyGde2MenuTree*) self;

	callback = pygde2menu_tree_callback_new(self, pycallback, pyuser_data);

	tree->callbacks = g_slist_append(tree->callbacks, callback);

	{
		Gde2MenuTreeDirectory* dir = gde2menu_tree_get_root_directory(tree->tree);
		if (dir)
		{
			gde2menu_tree_item_unref(dir);
		}
	}

	gde2menu_tree_add_monitor(tree->tree, (Gde2MenuTreeChangedFunc) pygde2menu_tree_handle_monitor_callback, callback);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pygde2menu_tree_remove_monitor(PyObject* self, PyObject* args)
{
	PyGde2MenuTree* tree;
	PyObject* pycallback;
	PyObject* pyuser_data;
	GSList* tmp;

	if (!PyArg_ParseTuple(args, "O|O:gde2menu.Tree.remove_monitor", &pycallback, &pyuser_data))
	{
		return NULL;
	}

	tree = (PyGde2MenuTree*) self;

	tmp = tree->callbacks;

	while (tmp != NULL)
	{
		PyGde2MenuTreeCallback* callback = tmp->data;
		GSList* next = tmp->next;

		if (callback->callback  == pycallback && callback->user_data == pyuser_data)
		{
			tree->callbacks = g_slist_delete_link(tree->callbacks, tmp);
			pygde2menu_tree_callback_free(callback);
		}

		tmp = next;
	}

	Py_INCREF(Py_None);

	return Py_None;
}

static void pygde2menu_tree_dealloc(PyGde2MenuTree* self)
{
	g_slist_foreach(self->callbacks, (GFunc) pygde2menu_tree_callback_free, NULL);
	g_slist_free(self->callbacks);
	self->callbacks = NULL;

	if (self->tree != NULL)
	{
		gde2menu_tree_unref(self->tree);
	}

	self->tree = NULL;

	PyObject_DEL(self);
}

static PyObject* pygde2menu_tree_getattro(PyGde2MenuTree* self, PyObject* py_attr)
{
	if (PyString_Check(py_attr))
	{
		char* attr;

		attr = PyString_AsString(py_attr);

		if (!strcmp(attr, "__members__"))
		{
			return Py_BuildValue("[sss]", "root", "menu_file", "sort_key");
		}
		else if (!strcmp(attr, "root"))
		{
			return pygde2menu_tree_get_root_directory((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "menu_file"))
		{
			return pygde2menu_tree_get_menu_file((PyObject*) self, NULL);
		}
		else if (!strcmp(attr, "sort_key"))
		{
			return pygde2menu_tree_get_sort_key((PyObject*) self, NULL);
		}
	}

	return PyObject_GenericGetAttr((PyObject*) self, py_attr);
}

static int pygde2menu_tree_setattro(PyGde2MenuTree* self, PyObject* py_attr, PyObject* py_value)
{
	PyGde2MenuTree* tree;

	tree = (PyGde2MenuTree*) self;

	if (PyString_Check(py_attr))
	{
		char* attr;

		attr = PyString_AsString(py_attr);

		if (!strcmp(attr, "sort_key"))
		{
			if (PyInt_Check(py_value))
			{
				int sort_key;

				sort_key = PyInt_AsLong(py_value);

				if (sort_key < GDE2MENU_TREE_SORT_FIRST || sort_key > GDE2MENU_TREE_SORT_LAST)
				{
					return -1;
				}

				gde2menu_tree_set_sort_key(tree->tree, sort_key);

				return 0;
			}
		}
	}

	return -1;
}

static struct PyMethodDef pygde2menu_tree_methods[] = {
	{"get_menu_file", pygde2menu_tree_get_menu_file, METH_VARARGS},
	{"get_root_directory", pygde2menu_tree_get_root_directory, METH_VARARGS},
	{"get_directory_from_path", pygde2menu_tree_get_directory_from_path, METH_VARARGS},
	{"get_sort_key", pygde2menu_tree_get_sort_key, METH_VARARGS},
	{"set_sort_key", pygde2menu_tree_set_sort_key, METH_VARARGS},
	{"add_monitor", pygde2menu_tree_add_monitor, METH_VARARGS},
	{"remove_monitor", pygde2menu_tree_remove_monitor, METH_VARARGS},
	{NULL, NULL, 0}
};

static PyTypeObject PyGde2MenuTree_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                                    /* ob_size */
	"gde2menu.Tree",                      /* tp_name */
	sizeof(PyGde2MenuTree),               /* tp_basicsize */
	0,                                    /* tp_itemsize */
	(destructor) pygde2menu_tree_dealloc, /* tp_dealloc */
	(printfunc) 0,                        /* tp_print */
	(getattrfunc) 0,                      /* tp_getattr */
	(setattrfunc) 0,                      /* tp_setattr */
	(cmpfunc) 0,                          /* tp_compare */
	(reprfunc) 0,                         /* tp_repr */
	0,                                    /* tp_as_number */
	0,                                    /* tp_as_sequence */
	0,                                    /* tp_as_mapping */
	(hashfunc) 0,                         /* tp_hash */
	(ternaryfunc) 0,                      /* tp_call */
	(reprfunc) 0,                         /* tp_str */
	(getattrofunc) pygde2menu_tree_getattro, /* tp_getattro */
	(setattrofunc) pygde2menu_tree_setattro, /* tp_setattro */
	0,                                    /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                   /* tp_flags */
	NULL,                                 /* Documentation string */
	(traverseproc) 0,                     /* tp_traverse */
	(inquiry) 0,                          /* tp_clear */
	(richcmpfunc) 0,                      /* tp_richcompare */
	0,                                    /* tp_weaklistoffset */
	(getiterfunc) 0,                      /* tp_iter */
	(iternextfunc) 0,                     /* tp_iternext */
	pygde2menu_tree_methods,              /* tp_methods */
	0,                                    /* tp_members */
	0,                                    /* tp_getset */
	(PyTypeObject*) 0,                    /* tp_base */
	(PyObject*) 0,                        /* tp_dict */
	0,                                    /* tp_descr_get */
	0,                                    /* tp_descr_set */
	0,                                    /* tp_dictoffset */
	(initproc) 0,                         /* tp_init */
	0,                                    /* tp_alloc */
	0,                                    /* tp_new */
	0,                                    /* tp_free */
	(inquiry) 0,                          /* tp_is_gc */
	(PyObject*) 0,                        /* tp_bases */
};

static PyGde2MenuTree* pygde2menu_tree_wrap(Gde2MenuTree* tree)
{
	PyGde2MenuTree* retval;

	if ((retval = gde2menu_tree_get_user_data(tree)) != NULL)
	{
		Py_INCREF(retval);
		return retval;
	}

	if (!(retval = (PyGde2MenuTree*) PyObject_NEW(PyGde2MenuTree, &PyGde2MenuTree_Type)))
	{
		return NULL;
	}

	retval->tree = gde2menu_tree_ref(tree);
	retval->callbacks = NULL;

	gde2menu_tree_set_user_data(tree, retval, NULL);

	return retval;
}

static PyObject* pygde2menu_lookup_tree(PyObject* self, PyObject* args)
{
	char* menu_file;

	Gde2MenuTree* tree;
	PyGde2MenuTree* retval;
	int flags;

	flags = GDE2MENU_TREE_FLAGS_NONE;

	if (!PyArg_ParseTuple(args, "s|i:gde2menu.lookup_tree", &menu_file, &flags))
	{
		return NULL;
	}

	if (!(tree = gde2menu_tree_lookup(menu_file, flags)))
	{
		Py_INCREF(Py_None);
		return Py_None;
	}

	retval = pygde2menu_tree_wrap(tree);

	gde2menu_tree_unref(tree);

	return (PyObject*) retval;
}

static struct PyMethodDef pygde2menu_methods[] = {
	{"lookup_tree", pygde2menu_lookup_tree, METH_VARARGS},
	{NULL, NULL, 0 }
};

void initgde2menu(void);

DL_EXPORT(void) initgde2menu(void)
{
	PyObject* mod;

	mod = Py_InitModule4("gde2menu", pygde2menu_methods, 0, 0, PYTHON_API_VERSION);

	#define REGISTER_TYPE(t, n) G_STMT_START \
	{ \
		t.ob_type = &PyType_Type; \
		PyType_Ready(&t); \
		PyModule_AddObject(mod, n, (PyObject*) &t); \
	} G_STMT_END

	REGISTER_TYPE(PyGde2MenuTree_Type,     "Tree");
	REGISTER_TYPE(PyGde2MenuTreeItem_Type, "Item");

	#define REGISTER_ITEM_TYPE(t, n) G_STMT_START \
	{ \
		t.ob_type = &PyType_Type; \
		t.tp_base = &PyGde2MenuTreeItem_Type; \
		PyType_Ready(&t); \
		PyModule_AddObject(mod, n, (PyObject*) &t); \
	} G_STMT_END

	REGISTER_ITEM_TYPE(PyGde2MenuTreeDirectory_Type, "Directory");
	REGISTER_ITEM_TYPE(PyGde2MenuTreeEntry_Type,     "Entry");
	REGISTER_ITEM_TYPE(PyGde2MenuTreeSeparator_Type, "Separator");
	REGISTER_ITEM_TYPE(PyGde2MenuTreeHeader_Type,    "Header");
	REGISTER_ITEM_TYPE(PyGde2MenuTreeAlias_Type,     "Alias");

	PyModule_AddIntConstant(mod, "TYPE_INVALID",   GDE2MENU_TREE_ITEM_INVALID);
	PyModule_AddIntConstant(mod, "TYPE_DIRECTORY", GDE2MENU_TREE_ITEM_DIRECTORY);
	PyModule_AddIntConstant(mod, "TYPE_ENTRY",     GDE2MENU_TREE_ITEM_ENTRY);
	PyModule_AddIntConstant(mod, "TYPE_SEPARATOR", GDE2MENU_TREE_ITEM_SEPARATOR);
	PyModule_AddIntConstant(mod, "TYPE_HEADER",    GDE2MENU_TREE_ITEM_HEADER);
	PyModule_AddIntConstant(mod, "TYPE_ALIAS",     GDE2MENU_TREE_ITEM_ALIAS);

	PyModule_AddIntConstant(mod, "FLAGS_NONE",                GDE2MENU_TREE_FLAGS_NONE);
	PyModule_AddIntConstant(mod, "FLAGS_INCLUDE_EXCLUDED",    GDE2MENU_TREE_FLAGS_INCLUDE_EXCLUDED);
	PyModule_AddIntConstant(mod, "FLAGS_SHOW_EMPTY",          GDE2MENU_TREE_FLAGS_SHOW_EMPTY);
	PyModule_AddIntConstant(mod, "FLAGS_INCLUDE_NODISPLAY",   GDE2MENU_TREE_FLAGS_INCLUDE_NODISPLAY);
	PyModule_AddIntConstant(mod, "FLAGS_SHOW_ALL_SEPARATORS", GDE2MENU_TREE_FLAGS_SHOW_ALL_SEPARATORS);

	PyModule_AddIntConstant(mod, "SORT_NAME",         GDE2MENU_TREE_SORT_NAME);
	PyModule_AddIntConstant(mod, "SORT_DISPLAY_NAME", GDE2MENU_TREE_SORT_DISPLAY_NAME);
}
