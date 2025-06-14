	This is a project for the computer graphics class in Cracow University of Technology for the erasmus 2025 program.
	Project contributors are
	  Ahmet Yasin Pekdemir
	  Michele Zazzaretti
	  Alejandro Roman Lopez
	
	This is a program that saves and loads images in different formats and compressions.

 	--HOW TO USE--
	Load images with a-s-d-f-g-h-j-k-l-z-x-c
	Clear screen with b

	Press 1 to select dithering and process options
	Press 2 to process loaded the image.
 		Left is the loaded original image and right is the processed image
	Press 3 to save the right image
 	Press 4 to load the saved image to the left


	- File Format:
		Header:
			ID: 2 byte
			Width:       2 bytes
			Height:      2 bytes
			Mode:        1 byte
			Dithering:   1 byte
			Compression: 1 byte
			Data Size:   4 bytes

	- Image is read by snaking around. 
	
	- Compression is both byteRun and RLE, whichever is smaller, 
		uncompressed could be smaller as well.
	- Input alphabet is 6-bit
	- Imposed Palatte:
		6-bit colour data RRGGBB
		
	-Dedicated Palatte:
		When an image has less than 64 colours
		The image remains unchanged. Still 24bits
		This file is saved with the same header but after that it has the 64 colour palette. 
			192 bytes of the dedicated palatte, after which the image is saved as an index to the palatte
		Header + dedicated palatte + image indexed to the palatte
	
	
	
	
	What will the code do:
		1- Asks the user if it wants it dithered, Y/N
			1.1 the user is asked what method of dithering, 1 for Bayer, 2 for Floyd-Steinberg
				1.1.1 if 1 dithering mode is set to 1, Bayer 4x4
				1.1.2 if 2 dithering mode is set to 2, Floyd-Steinberg
				
		2- Asks the user what colour mode it should be, 1-2-3-4
			1- imposed palatte
			2- imposed greyscale
			3- dedicated greyscale
			4- dedicated palatte
		
		2.1- If dedicated options are chosen, it checks how many colours there are
			 if when counting it goes over 64 it stops checking and decides it's not dedicated
				gives the user an error and asks again to chose another option
			 if there are less than 64 colours it changes the mode to dedicated options
			 After changing it to the dedicated options, write the palatte to the file
		
		3- IF NOT DEDICATED
		   Snake around and collect the image data.
			 Keep the size of the file for uncompressed
		   Snake around and collect the image data as byteRun ( 6-bit input alphabet )
			 Keep the size of the file for byteRun
		   Snake around and collect the image data as RLE     ( 6-bit input alphabet )
			 Keep the size of the file for RLE
			 
			3.1 Choose the smallest size method. Set the method in the header
				If it is not RLE;
		   Snake around and collect the image data as the smallest method
		   
		   SAVING IS DONE
		   WHEN LOADING JUST READ THE HEADER AND DO THE OPPOSITE CONVERSIONS
			
