#pragma once

#include "VTKSliceViewer.h"

#include <vtkImageData.h>
#include <vtkRenderer.h>

void alignCameraToSlice(vtkImageData* image,
                        vtkRenderer* renderer,
                        SliceViewer::SliceOrientation orientation,
                        int sliceIndex,
                        double preservedParallelScale);
