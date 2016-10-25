/******************************************************************************
Student Name: Ben Ventresca			Student Number: 0823887
Date: March 12th, 2014				Course Name: CIS-2750
Contact: bventres@uoguelph.ca		Description: vcfextension.c boiler plate functions for Python
*****************************************************************************/

#include <Python.h>
#include "vcutil.h"
#include "vcftool.h"

VcFile * filep;
static int cardIndex = 0;

PyObject *Vcf_readFile(PyObject *self, PyObject *args){

	FILE * openFile = NULL;
	char * filename;
	VcStatus status;
	status.lineto = 0;
	status.linefrom = 0;
	status.code = OK;

	if (!PyArg_ParseTuple(args, "s", &filename)){
		return Py_BuildValue("s", "file arguement is bad");
	}
	openFile = fopen(filename, "r");
	if (!openFile){
		return Py_BuildValue("s", "Can't open file");
	}

	filep = malloc(sizeof(VcFile));
	getUnfolded(NULL, NULL);
	
	status = readVcFile(openFile, filep);
	if (status.code != OK){
		return Py_BuildValue("s", "Bad vcards in the file");
	}

	fclose(openFile);

	return Py_BuildValue("s", "OK");
}


PyObject *Vcf_getCard(PyObject *self, PyObject *args){

	int i = 0;
	PyObject *card;

	if (cardIndex == filep->ncards){
		card = NULL;
		cardIndex = 0; //RESET YOUR FUCKING STATIC VARIABLES YA GOON
		return Py_BuildValue("s", NULL);
	}

	PyArg_ParseTuple(args, "O", &card);

	for (i = 0; i < filep->cardp[cardIndex]->nprops; i++){
		PyObject * temp = Py_BuildValue("(i, s, s, s)", filep->cardp[cardIndex]->prop[i].name,
			filep->cardp[cardIndex]->prop[i].parval, filep->cardp[cardIndex]->prop[i].partype,
			filep->cardp[cardIndex]->prop[i].value);
		PyList_Append(card, temp);
	}
	cardIndex += 1;

	return Py_BuildValue("O", card);
}

PyObject *Vcf_freeFile(PyObject *self, PyObject *args){

	freeVcFile(filep);

	return Py_BuildValue("s", "OK");
}

PyObject *Vcf_writeFile(PyObject *self, PyObject *args){

	char * filename;
	FILE * writeFile;
	PyObject *cards, * item, * prop;
	int i = 0, j = 0;

	int name = 0;
	char * parval = NULL, * partype = NULL, * value = NULL;

	freeVcFile(filep);//FIXTAG ADDED THIS WITHOUT TESTING IT
	filep = malloc(sizeof(VcFile));
	filep->ncards = 0;

	if (!PyArg_ParseTuple(args, "sO", &filename, &cards)){
		return Py_BuildValue("s", "file name or cards arguement is bad");
	}
	writeFile = fopen(filename, "w");
	if (!writeFile){
		return Py_BuildValue("s", "Can't open the writing file");
	}
	int m = PyList_Size(cards);
	filep->cardp = NULL;
	filep->cardp = malloc(sizeof(Vcard*)*m+10);

	for (i = 0; i < m; i++){
		item = PyList_GetItem(cards, i);
		PyArg_ParseTuple(args, "O", &item);

		filep->ncards += 1;
		//fprintf(stdout, "card# = %d\n", filep->ncards);

		filep->cardp[i] = malloc(sizeof(Vcard)*1000);
		filep->cardp[i]->nprops = 0;

		for (j = 0; PyList_GetItem(item, j) != NULL; j++){
			filep->cardp[i]->nprops += 1;
			prop = PyList_GetItem(item, j);

			PyArg_ParseTuple(prop, "izzz", &name, &parval, &partype, &value);

			filep->cardp[i]->prop[j].name = name;
			filep->cardp[i]->prop[j].parval = parval;
			filep->cardp[i]->prop[j].partype = partype;
			filep->cardp[i]->prop[j].value = value;
		}
	}

	writeVcFile(writeFile, filep);
	
	return Py_BuildValue("s", "OK");
}

static PyMethodDef vcfMethods[] = {
	{"readFile", Vcf_readFile, METH_VARARGS},
	{"getCard", Vcf_getCard, METH_VARARGS},
	{"freeFile", Vcf_freeFile, METH_NOARGS},
	{"writeFile", Vcf_writeFile, METH_VARARGS},
	{NULL, NULL} 
};

static struct PyModuleDef vcfModuleDef = {
	PyModuleDef_HEAD_INIT,
	"Vcf", //enable "import Vcf"
	NULL, //omit module documentation
	-1, //module keeps state in global variables
	vcfMethods //link module name "Vcf" to methods table 
};

PyMODINIT_FUNC PyInit_Vcf(void){
	PyModule_Create(&vcfModuleDef);
}
