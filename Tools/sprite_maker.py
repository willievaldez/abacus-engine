from PIL import ImageTk, Image
import PySimpleGUI as sg
import numpy
import os

def format_ofile_name(fname):
	fname = os.path.basename(fname)
	fname = fname.split('.')[0]
	fname += '.png' 
	return fname

window = sg.Window('Sprite Maker 1.0'). Layout(
	[
		[sg.Text('Filename')],
		[sg.InputText(''), sg.FileBrowse()], 		 							 # 0 or Browse
		[sg.Button('Add')],													     # Add (event)
		[sg.Canvas(size=(50,50), background_color='black', key='final_canvas')], # final_canvas
		[sg.Text('Save As'), sg.InputText('.png')],  						 	 # 1 or Browse0
		[sg.Button(button_text="Save")]											 # Save (event)
	])

src_image = None
final_image = None

while True:
	event, output = window.Read()
	print(event)
	print(output)

	if event is None:
		break

	final_canvas = window.FindElement('final_canvas').TKCanvas

	if event == 'Add':
		src_image = Image.open(output[0])
		if (final_image == None):
			final_image = src_image
		else:
			# concatenate new src_image to final_image
			final_np_im = numpy.array(final_image)
			new_np_im = numpy.array(src_image)
			final_np_im = numpy.concatenate((final_np_im, new_np_im), axis=1)
			final_image = Image.fromarray(final_np_im)

		imTk = ImageTk.PhotoImage(final_image)
		final_canvas.config(width=imTk.width(), height=imTk.height())
		final_canvas.create_image((imTk.width()/2,imTk.height()/2), image=imTk)

	elif event == "Save" and final_image != None:
		fname = format_ofile_name(output[1])
		final_image.save(fname)