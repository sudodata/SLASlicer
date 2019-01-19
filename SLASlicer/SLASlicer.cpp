#include <iostream>
#include <array>

using namespace std;

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "STLSlicer.h"

int main()
{
	// hard code bean bvalues for now (assuming 0.05mm slice thickness)
	// TODO: get from a JSON config file (or command line?)
	array<size_t, 3> volSize = { 1440, 2560, 800 };														// bean
	Point3D buildVol = { (fp_t)68, (fp_t)120, (fp_t)160 };												// bean (mm)
	Point3D voxelSize = { (fp_t)0.05, (fp_t)0.05, (fp_t)0.05 };											// bean (mm) (or should X,Y be buildVol / volSize)?
	Point3D origin = { (fp_t)(volSize[0] / 2.0 - 0.5), (fp_t)(volSize[1] / 2.0 - 0.5), (fp_t)(-0.5) };	// fractional index

	STLGeometry geometry;
	cin >> geometry;

	// hard code for the test model
	// TODO: also encode transformations in the JSON config file?
	geometry.Scale((fp_t)0.5, (fp_t)0.5, (fp_t)0.5);

	STLSlicer slicer(&geometry, volSize, buildVol, origin, voxelSize);

	cv::Mat slice((int)volSize[1], (int)volSize[0], CV_8U);

	fp_t zPosition = - origin[2]*voxelSize[2];
	for (int z = 0; z < buildVol[2]; z++)
	{

		if (slicer.Slice(&slice, zPosition))
		{
			stringstream filename;
			// TODO: hardcode file name for now -- add to config file?
			filename << "slices\\slice" << z << ".png";

			try
			{
				// TODO: optionally use morphological open/close operations to remove fine structures
				imwrite(filename.str().c_str(), slice);
			}
			catch (runtime_error& ex)
			{
				cerr << "Exception converting image to PNG format: " << ex.what() << endl;
				return 1;
			}
			zPosition += voxelSize[2];
		}
		else
		{
			// done slicing
			// TODO: do whatever clean-up necessary (eg zip the directory)
			return 0;
		}
	}
}

