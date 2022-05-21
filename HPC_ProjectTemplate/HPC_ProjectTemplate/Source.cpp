#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;




int ImageWidth = 4, ImageHeight = 4;
int L = 0;

int* inputImage(int* w, int* h, System::String^ imagePath) //put the size of image in w & h
{
	int* input;

	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height*BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i*BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("D://For University//HPC_ProjectTemplate//HPC_ProjectTemplate//Data//OutPut//" + index + ".png");
	cout << "result Image Saved " << index << endl;
}
//O(n2)
void ConvertFrom1D_to_2D(int* arr_1D, int** arr_2D)
{
	int indexFor1D = 0;

	//Loop to transfer values from 1D to 2D
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{	
			arr_2D[i][j] = arr_1D[indexFor1D++];
		}
	}

}
//O(1)
int checkifOutOfBounds(int i, int j, int** arr_2D)
{
	int value;
	if (i<0 || i>ImageHeight - 1 || j<0 || j>ImageWidth - 1)
	{
		value = 0;
		return value;
	}
	else
	{
		
		return arr_2D[i][j];
	}
}
//O(1)
int ConvertIndex(int i, int j, int** arr_2d)
{
	double ArrOfValues[9];
	double v = 0.1;
	ArrOfValues[0] = checkifOutOfBounds(i - 1, j - 1, arr_2d);
	//cout <<ArrOfValues[0] << endl;
	ArrOfValues[1] = checkifOutOfBounds(i - 1, j, arr_2d);
	//cout <<ArrOfValues[1] << endl;
	ArrOfValues[2] = checkifOutOfBounds(i - 1, j + 1, arr_2d);
	//cout <<ArrOfValues[2] << endl;
	ArrOfValues[3] = checkifOutOfBounds(i, j - 1, arr_2d);
	//cout <<ArrOfValues[3]<<endl;
	ArrOfValues[4] = checkifOutOfBounds(i, j, arr_2d);
	//cout <<ArrOfValues[4]<<endl;
	ArrOfValues[5] = checkifOutOfBounds(i, j + 1, arr_2d);
	//cout <<ArrOfValues[5]<<endl;
	ArrOfValues[6] = checkifOutOfBounds(i + 1, j - 1, arr_2d);
	//cout <<ArrOfValues[6]<<endl;
	ArrOfValues[7] = checkifOutOfBounds(i + 1, j, arr_2d);
	//cout <<ArrOfValues[7]<<endl;
	ArrOfValues[8] = checkifOutOfBounds(i + 1, j + 1, arr_2d);
	//cout <<ArrOfValues[8]<<endl;
	double value = ((ArrOfValues[0] * v) + (ArrOfValues[1] * v) + (ArrOfValues[2] * v)) + ((ArrOfValues[3] * v) + (ArrOfValues[4]* v) + (ArrOfValues[5] * v)) + ((ArrOfValues[6] * v) + (ArrOfValues[7] * v) + (ArrOfValues[8] * v));
	//cout << value << endl;
	return int(value);	

}
//O(n2)
void convertFrom2D_TO_1D(int** arr_2d, int* arr)
{
	int k = 0;
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			arr[k++] = arr_2d[i][j];
		}
	}
	
}
int main()
{
	
	int start_s, stop_s, TotalTime = 0;
	System::String^ imagePath;
	std::string img;
	img = "D://For University//HPC_ProjectTemplate//HPC_ProjectTemplate//Data//Input//N.png";

	imagePath = marshal_as<System::String^>(img);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);


	//Start MyCode
	////////////////////////////////////////////////////////////

	//Initizalize 2D Array to transfer data from 1D to 2D array
	int** arr_2D;
	arr_2D = new int* [ImageHeight];
	//O(N)
	for (int i = 0; i < ImageHeight; i++)
	{
		arr_2D[i] = new int[ImageWidth];
	}

	//Convert form 1D to 2D
	//O(N2)
	ConvertFrom1D_to_2D(imageData, arr_2D);

	//Initialize new 2D array to recieve on it the calculation between the original image and Kernel
	int** New_arr_2D;
	New_arr_2D = new int* [ImageHeight];
	//O(N)
	for (int i = 0; i < ImageHeight; i++)
	{
		New_arr_2D[i] = new int[ImageWidth];
		
	}
	
	//Loop on Every Pixel on origial image and Apply on it "ConvertIndex" which do multiply between Pixel&neighbour and kernel
	//and but the result on the new_arr_2D
	//O(N2)
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			New_arr_2D[i][j] = ConvertIndex(i, j, arr_2D);
			//cout << New_arr_2D[i][j] << endl;
		}
	}
	//int* New_arr_1D = new int[ImageHeight * ImageWidth];

	//O(N2)
	convertFrom2D_TO_1D(New_arr_2D, imageData);
	/*for (int i = 0; i < ImageHeight * ImageWidth; i++)
	{
		
	{*/

	//End my code
	//////////////////////////////////////////////////////

	start_s = clock();
	createImage(imageData, ImageWidth, ImageHeight, 4);
	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;

	free(imageData);
	system("pause");

	
	return 0;

}



