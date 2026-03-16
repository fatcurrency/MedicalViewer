#include "SliceCameraUtils.h"

#include <algorithm>

void alignCameraToSlice(vtkImageData* image,
                        vtkRenderer* renderer,
                        SliceViewer::SliceOrientation orientation,
                        int sliceIndex,
                        double preservedParallelScale)
{
    if (!image || !renderer)
        return;

    auto* camera = renderer->GetActiveCamera();
    if (!camera)
        return;

    int extent[6];
    image->GetExtent(extent);

    double origin[3] = {0.0, 0.0, 0.0};
    double spacing[3] = {1.0, 1.0, 1.0};
    image->GetOrigin(origin);
    image->GetSpacing(spacing);

    const double cx = origin[0] + 0.5 * static_cast<double>(extent[0] + extent[1]) * spacing[0];
    const double cy = origin[1] + 0.5 * static_cast<double>(extent[2] + extent[3]) * spacing[1];
    const double cz = origin[2] + 0.5 * static_cast<double>(extent[4] + extent[5]) * spacing[2];
    const double distance = std::max(1.0, camera->GetDistance());

    camera->ParallelProjectionOn();

    switch (orientation)
    {
        case SliceViewer::Axial:
        {
            const double z = origin[2] + static_cast<double>(sliceIndex) * spacing[2];
            camera->SetFocalPoint(cx, cy, z);
            camera->SetPosition(cx, cy, z + distance);
            camera->SetViewUp(0.0, 1.0, 0.0);
            break;
        }
        case SliceViewer::Coronal:
        {
            const double y = origin[1] + static_cast<double>(sliceIndex) * spacing[1];
            camera->SetFocalPoint(cx, y, cz);
            camera->SetPosition(cx, y + distance, cz);
            camera->SetViewUp(0.0, 0.0, 1.0);
            break;
        }
        case SliceViewer::Sagittal:
        {
            const double x = origin[0] + static_cast<double>(sliceIndex) * spacing[0];
            camera->SetFocalPoint(x, cy, cz);
            camera->SetPosition(x + distance, cy, cz);
            camera->SetViewUp(0.0, 0.0, 1.0);
            break;
        }
    }

    if (preservedParallelScale > 0.0)
    {
        camera->SetParallelScale(preservedParallelScale);
    }

    renderer->ResetCameraClippingRange();
}
