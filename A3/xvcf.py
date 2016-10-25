#!/usr/bin/python3

#/******************************************************************************
#Student Name: Ben Ventresca		Student Number: 0823887
#Date: March 12th, 2014				Course Name: CIS-2750
#Contact: bventres@uoguelph.ca		Description: xvcf.py - our gui for A3
#*****************************************************************************/

import tkinter

from tkinter import *
from tkinter.messagebox import *
from tkinter.tix import *
from tkinter.constants import *
from tkinter.scrolledtext import *


from tkinter import filedialog
#from filedialog import askopenfilename
import re
from GMapData import *
import Vcf
import subprocess
import sys
import tempfile
import os

cards = []
root=Tk()
fView = Frame(root)
cView = Frame(root)
lView = Frame(root)

logText = ScrolledText(lView, height=6, width=100)

m = Menu(root)
#"File menu
filemenu = Menu(m, tearoff=0)
orgmenu = Menu(m, tearoff=0)
sortEnabled = 0
reverting = 0

#does the card view display
def fileCheck(select):
	global cards
	global selectedCard
	global selectCardIndex

	cardPanel.delete_all()
	#print(select)
	index = int(select)
	selectCardIndex = index
	#print(cards[index])
	selectedCard = cards[index]

	selectionEnums = ["BEGIN", "END", "VERSION", "N", "FN", "NICKNAME", "PHOTO", "BDAY", "ADR", "LABEL", "TEL", "EMAIL", "GEO", "TITLE", "ORG", "NOTE", "UID", "URL", "OTHER"]

	j = 0
	for prop in selectedCard:
		convertName = selectionEnums[prop[0]]
		#print(convertName)
		cardPanel.add(j, itemtype=tkinter.tix.TEXT, text=convertName)
		cardPanel.item_create(j, 1, itemtype=tkinter.tix.TEXT, text=prop[1])
		cardPanel.item_create(j, 2, itemtype=tkinter.tix.TEXT, text=prop[2])
		cardPanel.item_create(j, 3, itemtype=tkinter.tix.TEXT, text=prop[3])
		j += 1

def selectedProp(select):

	global selectedCard
	global selectedProp
	global belowProp
	global aboveProp
	global propIndex

	belowProp = -1
	aboveProp = -1

	max = 0
	for p in selectedCard:
		max += 1

	index = int(select)
	propIndex = index
	#print("max index is", max)
	selectedProp = selectedCard[index]

	if ( (index + 1) < max):
		belowProp = selectedCard[index+1]
	if ( (index - 1) > 0):
		aboveProp = selectedCard[index-1]


filePanel = HList(fView, columns=7, header=True, height=10, width=100, browsecmd=fileCheck, selectmode="extended")
cardPanel = HList(cView, columns=4, header=True, height=10, width=100, browsecmd=selectedProp, selectmode="extended")


#file menu programs

#open file program
def openProg():

	global filepath
	global filename
	global cards
	global reverting

	if (reverting == 0):
		filepath = filedialog.askopenfilename(parent=root)#get filepath 
		fileList = re.findall('[^/]*$', filepath) #find the file name specifically
		filename = fileList[0] #place filename in variable
	else:
		reverting = 0
		orgmenu.entryconfig(3, state=DISABLED)

	numCards = 0

	filePanel.delete_all()
	cardPanel.delete_all()
	
	readF = Vcf.readFile(filepath)#call reading the file from its path
	#print(readF)

	#this updates the log Display
	openF = open(filepath)
	#execute the vcftool -info command to the file we are reading
	cmd = subprocess.Popen(["./vcftool", "-info"], stdin=openF, stdout=subprocess.PIPE)
	cmd_out, cmd_err = cmd.communicate() #get the stdout
	openF.close() #close the filepath

	logText.insert(tkinter.INSERT, cmd_out) #insert the output to the log text

	#enable save, save as and append options
	filemenu.entryconfig(1, state=NORMAL)
	filemenu.entryconfig(2, state=NORMAL)
	filemenu.entryconfig(3, state=NORMAL)

	#enable organization buttons
	orgmenu.entryconfig(0, state=NORMAL)
	orgmenu.entryconfig(1, state=NORMAL)
	orgmenu.entryconfig(2, state=NORMAL)

	#this populates the file view
	card = []
	cards = []

	i = 0
	while card != None: #get the cards in a loop
		card = [] #reset the card list
		card = Vcf.getCard(card)
		#print("card = ", card)
		if card != None:
			cards.append(card)
			i = i + 1
	numCards = i

	#print("num cards = ", numCards)

	i = 0
	for curCard in cards:
		j = 0
		name = 0
		adr = 0
		tel = 0
		uid = 0
		photo = 0
		geo = 0
		url = 0
		flagText = ""

		if curCard != None: #FIXTAG, added this to avoid a traceback
			for prop in curCard:

				if (j == 0):
					filePanel.add(i, itemtype=tkinter.tix.TEXT, data=i+1, text=i+1)

				if prop[0] == 3 and name == 0: #3 means name
					#place the value into the proper column
					filePanel.item_create(i, 1, itemtype=tkinter.tix.TEXT, text=prop[3])
					name += 1
				elif prop[0] == 3:
					name += 1

				if prop[0] == 8: #8 means adr
					whole = prop[3].split(";")

					region = ""
					if len(whole) > 4:
						region = prop[3].split(";")[4] #get region
					if len(region) == 0: #if not region set to none
						region = "None"

					country = ""
					if len(whole) > 6:
						country = prop[3].split(";")[6] #get country
					if len(country) == 0: #if no country set to NOne
						country = "None"

					filePanel.item_create(i, 2, itemtype=tkinter.tix.TEXT, text=region)
					filePanel.item_create(i, 3, itemtype=tkinter.tix.TEXT, text=country)

					adr += 1;

				if prop[0] == 10: #10 means telephone
					tel += 1;

				if prop[0] == 16: #16 means uid
					uid = 1;

				if prop[0] == 6:#got photo
					photo = 1;

				if prop[0] == 12: #got geo
					geo = 1;

				if prop[0] == 17: #got url
					url = 1;

				j += 1 #increment property position

			filePanel.item_create(i, 4, itemtype=tkinter.tix.TEXT, text=adr)
			filePanel.item_create(i, 5, itemtype=tkinter.tix.TEXT, text=tel)

			if uid == 1:
				flagText += "C"
			else:
				flagText += "-"

			if name > 1:
				flagText += "M"
			else:
				flagText += "-"

			if url == 1:
				flagText += "U"
			else:
				flagText += "-"
			if photo == 1:
				flagText += "P"
			else:
				flagText += "-"
			if geo == 1:
				flagText += "G"
			else:
				flagText += "-"

			filePanel.item_create(i, 6, itemtype=tkinter.tix.TEXT, text=flagText)
			i += 1 #increment card position

	Vcf.freeFile()
	
#append to the file
def appendProg():

	#insert the output to the log text
	logText.insert(tkinter.INSERT, "Appending\n")



#save the file
def saveProg():
	
	global cards

	if (filepath):
		Vcf.writeFile(filename, cards)
	else:
		saveAsProg()

#save into a new file name specified 
def saveAsProg():

	savepath = filedialog.asksaveasfilename(initialfile = filename)
	Vcf.writeFile(savepath, cards);

#Organize menu programs

#sort the cards
def sortProg():

	global revertPath
	global filename
	global filepath
	global cards

	openF = open(filepath)
	#sort the cards
	os.system("./vcftool -sort < " + filepath + " > sortFile.vcf 2> logfile.txt")

	sortedPath = os.getcwd()
	sortedPath += "/sortFile.vcf" #get the path name

	logText.insert(tkinter.INSERT, "Sorting cards\n")

	#delete hlists of old list of cards
	filePanel.delete_all() 
	cardPanel.delete_all()

	readSorted = Vcf.readFile(sortedPath)#call reading the file from its path
	#print("read status = ", readSorted)

	os.system("./vcftool -info < " + sortedPath + " > log.txt")#call the info on the cards

	#read log text into the log window
	with open('log.txt', 'r') as content_file:
		content = content_file.read()

	logText.insert(END, content) #insert it
	logText.see(END)

	sCard = []
	sCards = []

	i = 0
	while sCard != None: #get the cards in a loop
		sCard = [] #reset the card list
		sCard = Vcf.getCard(sCard)
		#print("Scard = ", sCard)
		if sCard != None:
			sCards.append(sCard)
			i = i + 1
	snumCards = i

	i = 0
	for cursCard in sCards:
		j = 0
		name = 0
		adr = 0
		tel = 0
		uid = 0
		photo = 0
		geo = 0
		url = 0
		flagText = ""

		if cursCard != None: #FIXTAG, added this to avoid a traceback
			for sprop in cursCard:

				if (j == 0):
					filePanel.add(i, itemtype=tkinter.tix.TEXT, data=i+1, text=i+1)

				if sprop[0] == 3 and name == 0: #3 means name
					#place the value into the proper column
					filePanel.item_create(i, 1, itemtype=tkinter.tix.TEXT, text=sprop[3])
					name += 1
				elif sprop[0] == 3:
					name += 1

				if sprop[0] == 8: #8 means adr
					whole = sprop[3].split(";")

					region = ""
					if len(whole) > 4:
						region = sprop[3].split(";")[4] #get region
					if len(region) == 0: #if not region set to none
						region = "None"

					country = ""
					if len(whole) > 6:
						country = sprop[3].split(";")[6] #get country
					if len(country) == 0: #if no country set to NOne
						country = "None"

					filePanel.item_create(i, 2, itemtype=tkinter.tix.TEXT, text=region)
					filePanel.item_create(i, 3, itemtype=tkinter.tix.TEXT, text=country)

					adr += 1;

				if sprop[0] == 10: #10 means telephone
					tel += 1;

				if sprop[0] == 16: #16 means uid
					uid = 1;

				if sprop[0] == 6:#got photo
					photo = 1;

				if sprop[0] == 12: #got geo
					geo = 1;

				if sprop[0] == 17: #got url
					url = 1;

				j += 1 #increment property position

			filePanel.item_create(i, 4, itemtype=tkinter.tix.TEXT, text=adr)
			filePanel.item_create(i, 5, itemtype=tkinter.tix.TEXT, text=tel)

			if uid == 1:
				flagText += "C"
			else:
				flagText += "-"

			if name > 1:
				flagText += "M"
			else:
				flagText += "-"

			if url == 1:
				flagText += "U"
			else:
				flagText += "-"
			if photo == 1:
				flagText += "P"
			else:
				flagText += "-"
			if geo == 1:
				flagText += "G"
			else:
				flagText += "-"

			filePanel.item_create(i, 6, itemtype=tkinter.tix.TEXT, text=flagText)
			i += 1 #increment card position

	revertPath = filepath #make a backup
	orgmenu.entryconfig(3, state=NORMAL)
	filepath = sortedPath
	cards = sCards

#canonicalize the cards
def canonProg():

	global filename
	global filepath
	global cards

	openF = open(filepath)
	#sort the cards
	os.system("./vcftool -canon < " + filepath + " > canonFile.vcf 2> logfile.txt")

	canonPath = os.getcwd()
	canonPath += "/canonFile.vcf" #get the path name

	logText.insert(tkinter.INSERT, "Canonicalizing cards\n")

	#delete hlists of old list of cards
	filePanel.delete_all() 
	cardPanel.delete_all()

	readCanon = Vcf.readFile(canonPath)#call reading the file from its path
	#print("read status = ", readCanon)

	os.system("./vcftool -info < " + canonPath + " > log.txt")#call the info on the cards

	#read log text into the log window
	with open('log.txt', 'r') as content_file:
		content = content_file.read()

	logText.insert(END, content) #insert it
	logText.see(END)

	sCard = []
	sCards = []

	i = 0
	while sCard != None: #get the cards in a loop
		sCard = [] #reset the card list
		sCard = Vcf.getCard(sCard)
		#print("Scard = ", sCard)
		if sCard != None:
			sCards.append(sCard)
			i = i + 1
	snumCards = i

	i = 0
	for cursCard in sCards:
		j = 0
		name = 0
		adr = 0
		tel = 0
		uid = 0
		photo = 0
		geo = 0
		url = 0
		flagText = ""

		if cursCard != None: #FIXTAG, added this to avoid a traceback
			for sprop in cursCard:

				if (j == 0):
					filePanel.add(i, itemtype=tkinter.tix.TEXT, data=i+1, text=i+1)

				if sprop[0] == 3 and name == 0: #3 means name
					#place the value into the proper column
					filePanel.item_create(i, 1, itemtype=tkinter.tix.TEXT, text=sprop[3])
					name += 1
				elif sprop[0] == 3:
					name += 1

				if sprop[0] == 8: #8 means adr
					whole = sprop[3].split(";")

					region = ""
					if len(whole) > 4:
						region = sprop[3].split(";")[4] #get region
					if len(region) == 0: #if not region set to none
						region = "None"

					country = ""
					if len(whole) > 6:
						country = sprop[3].split(";")[6] #get country
					if len(country) == 0: #if no country set to NOne
						country = "None"

					filePanel.item_create(i, 2, itemtype=tkinter.tix.TEXT, text=region)
					filePanel.item_create(i, 3, itemtype=tkinter.tix.TEXT, text=country)

					adr += 1;

				if sprop[0] == 10: #10 means telephone
					tel += 1;

				if sprop[0] == 16: #16 means uid
					uid = 1;

				if sprop[0] == 6:#got photo
					photo = 1;

				if sprop[0] == 12: #got geo
					geo = 1;

				if sprop[0] == 17: #got url
					url = 1;

				j += 1 #increment property position

			filePanel.item_create(i, 4, itemtype=tkinter.tix.TEXT, text=adr)
			filePanel.item_create(i, 5, itemtype=tkinter.tix.TEXT, text=tel)

			if uid == 1:
				flagText += "C"
			else:
				flagText += "-"

			if name > 1:
				flagText += "M"
			else:
				flagText += "-"

			if url == 1:
				flagText += "U"
			else:
				flagText += "-"
			if photo == 1:
				flagText += "P"
			else:
				flagText += "-"
			if geo == 1:
				flagText += "G"
			else:
				flagText += "-"

			filePanel.item_create(i, 6, itemtype=tkinter.tix.TEXT, text=flagText)
			i += 1 #increment card position

	revertPath = filepath #make a backup
	orgmenu.entryconfig(3, state=NORMAL)
	filepath = canonPath
	cards = sCards

#select the cards based on certain properties
def selectProg():

	global filename
	global filepath
	global cards

	selectionWindow = Toplevel(root)
	selectionWindow.title("Select")

	pSel = IntVar()
	gSel = IntVar()
	uSel = IntVar()
	selButton = IntVar()

	photoSel = Checkbutton(selectionWindow, text="Photo", variable=pSel, onvalue= 1, offvalue=0)
	geoSel = Checkbutton(selectionWindow, text="Geo", variable=gSel, onvalue= 1, offvalue=0)
	urlSel = Checkbutton(selectionWindow, text="Url", variable=uSel, onvalue= 1, offvalue=0)
	photoSel.pack()
	geoSel.pack()
	urlSel.pack()
	selButton = Button(selectionWindow, text="Ok", command=lambda:selectionWindow.destroy(),width=10)
	
	selButton.pack()
	selectionWindow.geometry('200x150+300+300')
	selectionWindow.grab_set()
	selectionWindow.transient(root)
	selectionWindow.wait_window()

	#print("got psel, gsel, usel as:", pSel.get(), gSel.get(), uSel.get())

	selOptions = ""
	if (pSel.get() == 1):
		selOptions += "p"
	if (uSel.get() == 1):
		selOptions += "u"
	if (gSel.get() == 1):
		selOptions += "g"

	#print("sel options are", selOptions)

	openF = open(filepath)
	#print("./vcftool -select " + selOptions + " < " + filepath + " > selectFile.vcf 2> logfile.txt")
	#sort the cards
	os.system("./vcftool -select " + selOptions + " < " + filepath + " > selectFile.vcf 2> logfile.txt")

	selectPath = os.getcwd()
	selectPath += "/selectFile.vcf" #get the path name

	logText.insert(tkinter.INSERT, "Selecting cards\n")

	#delete hlists of old list of cards
	filePanel.delete_all() 
	cardPanel.delete_all()

	readSelect = Vcf.readFile(selectPath)#call reading the file from its path
	#print(readSelect)

	os.system("./vcftool -info < " + selectPath + " > log.txt")#call the info on the cards

	#read log text into the log window
	with open('log.txt', 'r') as content_file:
		content = content_file.read()

	logText.insert(END, content) #insert it
	logText.see(END)

	sCard = []
	sCards = []

	i = 0
	while sCard != None: #get the cards in a loop
		sCard = [] #reset the card list
		sCard = Vcf.getCard(sCard)
		#print("Scard = ", sCard)
		if sCard != None:
			sCards.append(sCard)
			i = i + 1
	snumCards = i

	i = 0
	for cursCard in sCards:
		j = 0
		name = 0
		adr = 0
		tel = 0
		uid = 0
		photo = 0
		geo = 0
		url = 0
		flagText = ""

		if cursCard != None: #FIXTAG, added this to avoid a traceback
			for sprop in cursCard:

				if (j == 0):
					filePanel.add(i, itemtype=tkinter.tix.TEXT, data=i+1, text=i+1)

				if sprop[0] == 3 and name == 0: #3 means name
					#place the value into the proper column
					filePanel.item_create(i, 1, itemtype=tkinter.tix.TEXT, text=sprop[3])
					name += 1
				elif sprop[0] == 3:
					name += 1

				if sprop[0] == 8: #8 means adr
					whole = sprop[3].split(";")

					region = ""
					if len(whole) > 4:
						region = sprop[3].split(";")[4] #get region
					if len(region) == 0: #if not region set to none
						region = "None"

					country = ""
					if len(whole) > 6:
						country = sprop[3].split(";")[6] #get country
					if len(country) == 0: #if no country set to NOne
						country = "None"

					filePanel.item_create(i, 2, itemtype=tkinter.tix.TEXT, text=region)
					filePanel.item_create(i, 3, itemtype=tkinter.tix.TEXT, text=country)

					adr += 1;

				if sprop[0] == 10: #10 means telephone
					tel += 1;

				if sprop[0] == 16: #16 means uid
					uid = 1;

				if sprop[0] == 6:#got photo
					photo = 1;

				if sprop[0] == 12: #got geo
					geo = 1;

				if sprop[0] == 17: #got url
					url = 1;

				j += 1 #increment property position

			filePanel.item_create(i, 4, itemtype=tkinter.tix.TEXT, text=adr)
			filePanel.item_create(i, 5, itemtype=tkinter.tix.TEXT, text=tel)

			if uid == 1:
				flagText += "C"
			else:
				flagText += "-"

			if name > 1:
				flagText += "M"
			else:
				flagText += "-"

			if url == 1:
				flagText += "U"
			else:
				flagText += "-"
			if photo == 1:
				flagText += "P"
			else:
				flagText += "-"
			if geo == 1:
				flagText += "G"
			else:
				flagText += "-"

			filePanel.item_create(i, 6, itemtype=tkinter.tix.TEXT, text=flagText)
			i += 1 #increment card position

	revertPath = filepath #make a backup
	orgmenu.entryconfig(3, state=NORMAL)
	filepath = selectPath #set this as the new file were working with
	cards = sCards

#undo the last change to the file cards
def undoProg():
	
	logText.insert(tkinter.INSERT, "Undoing last operation\n")
	logText.see(END)

	global revertPath
	global reverting

	reverting = 1

	filepath = revertPath
	openProg()

#Quit program function
def quitProg():
	
	exitCond = tkinter.messagebox.askquestion("Exit?", "Do you want to Exit?", icon='warning')
	if exitCond == 'yes':
		#print ("EXITING and deleting any temporary files!")
		killServers() #kill all the web servers
		mw.quit()
	else:
		logText.insert(tkinter.INSERT, "Not Exiting\n")
		logText.see(END)

#Shortcut Function for print
def showText(t):
	print (t)


#Our main menu declaration, the menu bar gets initialized with three menus
def mainWindowMenu(r):
	
	filemenu.add("command", label="Open", command=openProg)
	filemenu.add("command", label="Append", command=appendProg, state=DISABLED)
	filemenu.add("command", label="Save", command=saveProg, state=DISABLED)
	filemenu.add("command", label="Save As", command=saveAsProg, state=DISABLED)
	filemenu.add("command", label="Exit", command=quitProg)

	#Organize menu
	orgmenu.add("command", label="Sort", command=sortProg, state=DISABLED)
	orgmenu.add("command", label="Canonicalize", command=canonProg, state=DISABLED)
	orgmenu.add("command", label="Select", command=selectProg, state=DISABLED)
	orgmenu.add("command", label="Undo", command=undoProg, state=DISABLED)

	#"Help"
	helpmenu = Menu(m, tearoff=0) #if you make it tearoff 1, you can move the menu
	#Tearoff, run this program, Then drag the dotted line in the help menu. Very cool! =D
	helpmenu.add("command", label="Card flags and colours", command=aboutWindow)
	helpmenu.add("command", label="About xvcf", command=aboutProg)

	#Adds to menu
	m.add("cascade",menu=filemenu, label="File")
	m.add("cascade",menu=orgmenu, label="Organize")
	m.add("cascade",menu=helpmenu, label="Help")

	return m

#the about windows
def aboutWindow():
	#showinfo pops up an entire new window. Alert in Java. 
	showinfo("Card flags and colours...", "Flags: \nC = card is in canonical form\n\
M = card has multiple same mandatory properties.\n\
U, P, G = card has at least 1 URL, PHOTO, or GEO property.\n\nColour card statuses:\nGreen = card is canonical\n\
Red = card needs fixing (has multiple mandatory properties, etc)\nYellow = default state if neither Red or Green")

#about the program and developer
def aboutProg():
	showinfo("About Xvcf", "Xvcf is a graphic interface for\nviewing, creating and editing vCards of version 3.0\n\
		\nWritten by Ben Ventresca")

#the file view panel
def fileView(root):	
	fView.configure(bg = "white")
	fView.pack()

	fileLabel=Label(fView, text="File View:", foreground="black") 
	fileLabel.configure(background="white")
	fileLabel.pack(side=TOP, anchor=W)
	
	filePanel.header_create("0", text="Card#")
	filePanel.header_create("1", text="Name")
	filePanel.header_create("2", text="Region")
	filePanel.header_create("3", text="Country")
	filePanel.header_create("4", text="#ADR")
	filePanel.header_create("5", text="#TEL")
	filePanel.header_create("6", text="Flags")

	scroll = Scrollbar(fView)
	scroll.pack(side=RIGHT, fill=Y)

	fButtonsPanel = Frame(root)
	fButtonsPanel.pack()

	filePanel.pack()

	mapSelectB = Button(fButtonsPanel, text="Map Selected", command=mapButton)
	mapSelectB.grid(row=2, column=1)
	resetMapB = Button(fButtonsPanel, text="Reset Map", command=resetButton)
	resetMapB.grid(row=2, column=2)
	browseSelectB = Button(fButtonsPanel, text="Browse Selected", command=browseButton)
	browseSelectB.grid(row=2, column=3)
	delSelectB = Button(fButtonsPanel, text="Delete Selected", command=deleteButton)
	delSelectB.grid(row=2, column=4)
	addCardB = Button(fButtonsPanel, text="Add card", command=addButton)
	addCardB.grid(row=2, column=5)

#file view buttons below

#the mapping button
def mapButton():

	global selectedCard

	gotGeo = 0
	gotPhoto = 0
	gotAdr = 0
	name = ""
	geo = ""
	photo = ""
	address = ""
	for prop in selectedCard:
		if (prop[0] == 3):
			name = prop[3]
		if (prop[0] == 12 and gotGeo == 0): #if property is geo
			geo = prop[3]
			gotGeo = 1
		if (prop[0] == 6 and gotPhoto == 0): #if property is photo
			photo = prop[3]
			gotPhoto = 1
		if (prop[0] == 8 and gotAdr == 0): #if property is address
			address = prop[3]
			gotAdr = 1

	geoS = geo.split(",")
	if (len(geoS) == 2):
		lat = float(geoS[0])#convert geos to floats
		lon = float(geoS[1])
	geoS = geo.split(";")
	if (len(geoS) == 2):
		lat = float(geoS[0])#convert geos to floats
		lon = float(geoS[1])

	gmd = GMapData( name, address, [lat,lon], 14 )
	gmd.addPoint( [lat,lon], photo, address )	# s.b. center of map
	gmd.addOverlay( 0, 1, 3 )	# single point, blue icon

	logText.insert(tkinter.INSERT, "Loading Map... Please wait\n")
	logText.see(END)
	
	#3. generate HTML to serve
	gmd.serve( "public_html/index.html" );

	#4. launch browser
	launchBrowser( "http://localhost:43887/" )

#reset
def resetButton():
	
	logText.insert(tkinter.INSERT, "Reset Map Points\n")
	logText.see(END)


def browseButton():
	
	global selectedCard

	url = ""
	for prop in selectedCard:
		#print(convertName)
		if (prop[0] == 17): #if its a url
			url = prop[3]
			break;
	if (len(url) > 1):
		logText.insert(tkinter.INSERT, "Loading Website please wait\n")
		logText.see(END)
		launchBrowser(url)
	else:
		logText.insert(tkinter.INSERT, "No website to load!\n")
		logText.see(END)
	

def deleteButton():#working tag

	global selectCardIndex
	global cards

	logText.insert(tkinter.INSERT, "Deleting Card\n")
	logText.see(END)
	filePanel.delete_entry(selectCardIndex)
	for i in range(selectCardIndex, len(cards)):
		if (i == selectCardIndex):
			cards[i] = "None"
		if (i+1 < len(cards)):
			#print(cards[i+1])
			cards[i], cards[i+1] = cards[i+1], cards[i]


def addButton():
	
	logText.insert(tkinter.INSERT, "Adding!\n")
	logText.see(END)

	addWindow = Toplevel(root)
	addWindow.title("Add Card")

	pSel = IntVar()
	gSel = IntVar()
	uSel = IntVar()
	selButton = IntVar()

	e = Entry(addWindow)
	e.pack()
	d = Entry(addWindow)
	d.pack()
	f = Entry(addWindow)
	f.pack()

	selButton = Button(addWindow, text="Ok", command=lambda:addWindow.destroy(),width=10)
	
	selButton.pack()
	addWindow.geometry('400x400+300+300')
	addWindow.grab_set()
	addWindow.transient(root)
	addWindow.wait_window()


def cardView(root):

	cView.configure(bg = "white")
	cView.pack()

	cardLabel=Label(cView, text="Card View:", foreground="black") 
	cardLabel.configure(background="white")
	cardLabel.pack(side=TOP, anchor=W)
	
	cardPanel.header_create("0", text="Name")
	cardPanel.header_create("1", text="Type")
	cardPanel.header_create("2", text="Value")
	cardPanel.header_create("3", text="Property Value")

	scroll = Scrollbar(cView)
	scroll.pack(side=RIGHT, fill=Y)

	cButtonsPanel = Frame(root)
	cButtonsPanel.pack()
	cardPanel.pack()

	upB = Button(cButtonsPanel, text="Up", command=upButton)
	upB.grid(row=2, column=1)
	downB = Button(cButtonsPanel, text="Down", command=downButton)
	downB.grid(row=2, column=2)
	addPropB = Button(cButtonsPanel, text="Add Property", command=addPropButton)
	addPropB.grid(row=2, column=3)
	delPropB = Button(cButtonsPanel, text="Delete Property", command=deletePropButton)
	delPropB.grid(row=2, column=4)
	commitB = Button(cButtonsPanel, text="Commit", command=commitButton)
	commitB.grid(row=2, column=5)
	revertB = Button(cButtonsPanel, text="Revert", command=revertButton)
	revertB.grid(row=2, column=6)



def upButton():

	global cards
	global selectedCard
	global selectCardIndex
	global propIndex

	cardPanel.delete_all()
	selectedCard = cards[selectCardIndex]

	selectionEnums = ["BEGIN", "END", "VERSION", "N", "FN", "NICKNAME", "PHOTO", "BDAY", "ADR", "LABEL", "TEL", "EMAIL", "GEO", "TITLE", "ORG", "NOTE", "UID", "URL", "OTHER"]

	selectedCard[propIndex], selectedCard[propIndex-1] = selectedCard[propIndex-1], selectedCard[propIndex]
	j = 0
	for prop in selectedCard:
		convertName = selectionEnums[prop[0]]
		cardPanel.add(j, itemtype=tkinter.tix.TEXT, text=convertName)
		cardPanel.item_create(j, 1, itemtype=tkinter.tix.TEXT, text=prop[1])
		cardPanel.item_create(j, 2, itemtype=tkinter.tix.TEXT, text=prop[2])
		cardPanel.item_create(j, 3, itemtype=tkinter.tix.TEXT, text=prop[3])
		j += 1

	logText.insert(tkinter.INSERT, "Moving property UP\n")
	logText.see(END)

def downButton():

	global cards
	global selectedCard
	global selectCardIndex
	global propIndex

	cardPanel.delete_all()
	selectedCard = cards[selectCardIndex]

	selectionEnums = ["BEGIN", "END", "VERSION", "N", "FN", "NICKNAME", "PHOTO", "BDAY", "ADR", "LABEL", "TEL", "EMAIL", "GEO", "TITLE", "ORG", "NOTE", "UID", "URL", "OTHER"]

	selectedCard[propIndex], selectedCard[propIndex+1] = selectedCard[propIndex+1], selectedCard[propIndex]
	j = 0
	for prop in selectedCard:
		convertName = selectionEnums[prop[0]]
		cardPanel.add(j, itemtype=tkinter.tix.TEXT, text=convertName)
		cardPanel.item_create(j, 1, itemtype=tkinter.tix.TEXT, text=prop[1])
		cardPanel.item_create(j, 2, itemtype=tkinter.tix.TEXT, text=prop[2])
		cardPanel.item_create(j, 3, itemtype=tkinter.tix.TEXT, text=prop[3])
		j += 1

	logText.insert(tkinter.INSERT, "Moving property DOWN\n")
	logText.see(END)


def addPropButton():

	logText.insert(tkinter.INSERT, "Add property\n")
	logText.see(END)

	addWindow = Toplevel(root)
	addWindow.title("Add Card")


	selButton = Button(addWindow, text="Ok", command=lambda:addWindow.destroy(),width=10)
	
	selButton.pack()
	addWindow.geometry('400x400+300+300')
	addWindow.grab_set()
	addWindow.transient(root)
	addWindow.wait_window()

def deletePropButton():

	global selectedProp
	global aboveProp
	global propIndex

	
	if (selectedProp[0] != 3 and selectedProp[0] != 4):
		logText.insert(tkinter.INSERT, "Delete property\n")
		logText.see(END)
		cardPanel.delete_entry(propIndex)
	else:
		logText.insert(tkinter.INSERT, "Can't Delete that property\n")
		logText.see(END)

def commitButton():

	logText.insert(tkinter.INSERT, "Commiting\n")
	logText.see(END)

def revertButton():
	logText.insert(tkinter.INSERT, "Revert\n")
	logText.see(END)


def logView(root):	
	lView.configure(bg="white")
	lView.pack()

	logLabel=Label(lView, text="Log Display:", foreground="black") 
	logLabel.configure(background="white")
	logLabel.pack(side=TOP, anchor=W)

	lButtonPanel = Frame(root)
	lButtonPanel.pack()

	logText.pack()

	clearB = Button(lButtonPanel, text="Clear", command=clearButton)
	clearB.grid(row=2, column=1)

def clearButton():

	logText.delete('1.0', END) #clear the log text area

def exitTop():

	exitCond = tkinter.messagebox.askquestion("Exit?", "Do you want to Exit?", icon='warning')
	if exitCond == 'yes':
		#print ("EXITING and deleting any temporary files!")
		killServers() #kill all the web servers
		mw.quit()
	else:
		logText.insert(tkinter.INSERT, "Not Exiting\n")
		logText.see(END)

def buildMainWindow():
	#Declares the general menu/root module. Sets size, title, and colour
	
	root.title("xvcf")
	root.minsize(700,650)
	root.configure(background="white")
	
	#Calls main Window Menu along with a .configure -> Shows the menu
	m=mainWindowMenu(root)
	root.configure(menu=m)

	startWebServer(43887) #start the web server

	fileView(root)
	cardView(root)
	logView(root)

	root.protocol("WM_DELETE_WINDOW", exitTop)

	return root

mw=buildMainWindow()
mainloop()
