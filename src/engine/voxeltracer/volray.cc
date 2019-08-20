#include "volray.h"


static int cycleValue(int val, int min, int max) {
    return val < min ? max : (val > max ? min : val);
}

static int cycleValue(int val, int max) {
    return val < 0 ? max : (val > max ? 0 : val);
}

static double cycleValue(double val, double max) {
    return val < 0 ? max : (val > max ? 0 : val);
}


Animation::Animation() {
    //keyFrames.add(new KeyFrame());
}

bool Animation::isLastFrame(int loopMode)
{
    bool isLastFrame = false;
    if(loopMode == 0 && currentKeyFrame + (currentSubFrame)/numSubFrames >= keyFrames.size()-1) {
        isLastFrame = true;
    }
    if(loopMode == 1 && currentKeyFrame + (currentSubFrame)/numSubFrames >= keyFrames.size()-1
    && (currentSubFrame)%numSubFrames >= numSubFrames-1) {
        isLastFrame = true;
    }

    return isLastFrame;
}

bool Animation::deltaFrame(int d, int loopMode)
{
    bool isLastFrame = false;
    if(loopMode == 0 && currentKeyFrame + (currentSubFrame+d)/numSubFrames >= keyFrames.size()-1) {
        currentKeyFrame =  keyFrames.size()-1;
        currentSubFrame = 0;
        d = 0;
        isLastFrame = true;
    }
    if(loopMode == 1 && currentKeyFrame + (currentSubFrame+d)/numSubFrames >= keyFrames.size()-1
    && (currentSubFrame+d)%numSubFrames >= numSubFrames-1) {
        currentKeyFrame =  keyFrames.size()-1;
        currentSubFrame = numSubFrames-1;
        d = 0;
        isLastFrame = true;
    }

    setFrame(currentKeyFrame, currentSubFrame+d);
    return isLastFrame;
}

void Animation::setFrame(int keyFrame, int subFrame)
{
    if(keyFrames.size() == 0) return;

    currentKeyFrame = cycleValue(keyFrame, keyFrames.size()-1);
    currentSubFrame = subFrame;

    while(currentSubFrame >= numSubFrames) {
        currentSubFrame -= numSubFrames;
        currentKeyFrame++;
        if(currentKeyFrame >= keyFrames.size()) {
            currentKeyFrame = 0;
        }
    }

    while(currentSubFrame < 0) {
        currentSubFrame += numSubFrames;
        currentKeyFrame--;
        if(currentKeyFrame < 0) {
            currentKeyFrame = keyFrames.size()-1;
        }
    }
    fraction = (double)currentSubFrame/(double)numSubFrames;
}

/*void setFrame(int keyFrame, double fraction)
{
currentKeyFrame = cycleValue(keyFrame, keyFrames.size()-1);
currentSubFrame = round(fraction*numSubFrames);
this->fraction = fraction;
}*/

void Animation::removeCurrentKeyFrame() {
    if(keyFrames.size() > 1) {
        keyFrames.erase(keyFrames.begin()+currentKeyFrame);
    }
    if(currentKeyFrame >= keyFrames.size()) {
        currentKeyFrame = keyFrames.size()-1;
    }
}

void Animation::reset() {
    keyFrames.clear();
    //keyFrames.add(new KeyFrame());
    currentKeyFrame = 0;
    currentSubFrame = 0;
}

double Animation::na(double a, double b) {
    while(b-a > PI) { b -= 2.0*PI; }
    while(a-b > PI) { b += 2.0*PI; }

    return b;
}

int modIndex(int ind, int n) {
    return (ind+n) % n;
}

void Animation::applyToScene(Scene3D &scene)
{
    int n = keyFrames.size();
    if(n == 0) return;

    f0 = keyFrames[modIndex(currentKeyFrame-1, n)];
    f1 = keyFrames[currentKeyFrame];
    f2 = keyFrames[modIndex(currentKeyFrame+1, n)];
    f3 = keyFrames[modIndex(currentKeyFrame+2, n)];

    scene.camCenterX = catmullRom(fraction, curvature, f0.camCenterX, f1.camCenterX, f2.camCenterX, f3.camCenterX);
    scene.camCenterY = catmullRom(fraction, curvature, f0.camCenterY, f1.camCenterY, f2.camCenterY, f3.camCenterY);
    scene.camCenterZ = catmullRom(fraction, curvature, f0.camCenterZ, f1.camCenterZ, f2.camCenterZ, f3.camCenterZ);
    double ax = f1.camRotX, ay = f1.camRotY;

    scene.camRotX = catmullRom(fraction, curvature, na(ax, f0.camRotX), ax, na(ax, f2.camRotX), na(ax, f3.camRotX));
    scene.camRotY = catmullRom(fraction, curvature, na(ay, f0.camRotY), ay, na(ay, f2.camRotY), na(ay, f3.camRotY));

    scene.camDistance = catmullRom(fraction, curvature, f0.camDistance, f1.camDistance, f2.camDistance, f3.camDistance);
    scene.redChannel = fraction < 0.5 ? f1.redChannel : f2.redChannel;
    scene.greenChannel = fraction < 0.5 ? f1.greenChannel : f2.greenChannel;
    scene.blueChannel = fraction < 0.5 ? f1.blueChannel : f2.blueChannel;
    scene.redFactor = clamp(catmullRom(fraction, curvature, f0.redFactor, f1.redFactor, f2.redFactor, f3.redFactor), 0, 1);
    scene.greenFactor = clamp(catmullRom(fraction, curvature, f0.greenFactor, f1.greenFactor, f2.greenFactor, f3.greenFactor), 0, 1);
    scene.blueFactor = clamp(catmullRom(fraction, curvature, f0.blueFactor, f1.blueFactor, f2.blueFactor, f3.blueFactor), 0, 1);
    scene.intensityFactor = max(0, catmullRom(fraction, curvature, f0.intensityFactor, f1.intensityFactor, f2.intensityFactor, f3.intensityFactor));
    scene.deltaOffset = max(0, catmullRom(fraction, curvature, f0.deltaOffset, f1.deltaOffset, f2.deltaOffset, f3.deltaOffset));
    scene.attenuationAlphaFactor = clamp(catmullRom(fraction, curvature, f0.attenuationAlphaFactor, f1.attenuationAlphaFactor, f2.attenuationAlphaFactor, f3.attenuationAlphaFactor), 0, 1);
    scene.colorAlphaFactor = clamp(catmullRom(fraction, curvature, f0.colorAlphaFactor, f1.colorAlphaFactor, f2.colorAlphaFactor, f3.colorAlphaFactor), 0, 1);
    scene.colorRayStrengthFactor = clamp(catmullRom(fraction, curvature, f0.colorRayStrengthFactor, f1.colorRayStrengthFactor, f2.colorRayStrengthFactor, f3.colorRayStrengthFactor), 0, 1);

    for(int i=0; i<scene.shadingModes.size(); i++) {
        scene.rayAttenuation[i] = catmullRom(fraction, curvature, f0.rayAttenuation[i], f1.rayAttenuation[i], f2.rayAttenuation[i], f3.rayAttenuation[i]);
    }

    scene.shadingMode = fraction < 0.5 ? f1.shadingMode : f2.shadingMode;
    scene.traverseMode = fraction < 0.5 ? f1.traverseMode : f2.traverseMode;

    scene.dynamicLevels = fraction < 0.5 ? f1.dynamicLevels : f2.dynamicLevels;
    scene.lockDynamicLevel = fraction < 0.5 ? f1.lockDynamicLevel : f2.lockDynamicLevel;

    scene.voxelGrid.minX = clamp(catmullRom(fraction, 0.1, f0.minX, f1.minX, f2.minX, f3.minX), 0, scene.voxelGrid.maxX-1);
    scene.voxelGrid.minY = clamp(catmullRom(fraction, 0.1, f0.minY, f1.minY, f2.minY, f3.minY), 0, scene.voxelGrid.maxY-1);
    scene.voxelGrid.minZ = clamp(catmullRom(fraction, 0.1, f0.minZ, f1.minZ, f2.minZ, f3.minZ), 0, scene.voxelGrid.maxZ-1);
    scene.voxelGrid.maxX = clamp(catmullRom(fraction, 0.1, f0.maxX, f1.maxX, f2.maxX, f3.maxX), scene.voxelGrid.minX+1, scene.voxelGrid.w);
    scene.voxelGrid.maxY = clamp(catmullRom(fraction, 0.1, f0.maxY, f1.maxY, f2.maxY, f3.maxY), scene.voxelGrid.minY+1, scene.voxelGrid.h);
    scene.voxelGrid.maxZ = clamp(catmullRom(fraction, 0.1, f0.maxZ, f1.maxZ, f2.maxZ, f3.maxZ), scene.voxelGrid.minZ+1, scene.voxelGrid.d);
}

void Animation::applyFromScene(int keyFrameIndex, bool insert, Scene3D &scene)
{
    keyFrameIndex = max(0, keyFrameIndex);
    KeyFrame *keyFrame = NULL;

    if(insert) {
        keyFrames.insert(keyFrames.begin()+currentKeyFrame, KeyFrame());
        keyFrame = &keyFrames[currentKeyFrame];
    }
    else {
        if(keyFrameIndex >= keyFrames.size()) {
            keyFrames.push_back(KeyFrame());
            keyFrame = &keyFrames[keyFrames.size()-1];
        }
        else {
            keyFrame = &keyFrames[keyFrameIndex];
        }
    }
    keyFrame->camCenterX = scene.camCenterX;
    keyFrame->camCenterY = scene.camCenterY;
    keyFrame->camCenterZ = scene.camCenterZ;
    keyFrame->camRotX = scene.camRotX;
    keyFrame->camRotY = scene.camRotY;
    keyFrame->camDistance = scene.camDistance;
    keyFrame->redChannel = scene.redChannel;
    keyFrame->greenChannel = scene.greenChannel;
    keyFrame->blueChannel = scene.blueChannel;
    keyFrame->redFactor = scene.redFactor;
    keyFrame->greenFactor = scene.greenFactor;
    keyFrame->blueFactor = scene.blueFactor;
    keyFrame->intensityFactor = scene.intensityFactor;
    keyFrame->deltaOffset = scene.deltaOffset;
    keyFrame->colorAlphaFactor = scene.colorAlphaFactor;
    keyFrame->colorRayStrengthFactor = scene.colorRayStrengthFactor;
    keyFrame->attenuationAlphaFactor = scene.attenuationAlphaFactor;

    for(int i=0; i<scene.shadingModes.size(); i++) {
        keyFrame->rayAttenuation[i] = scene.rayAttenuation[i];
    }

    keyFrame->shadingMode = scene.shadingMode;
    keyFrame->traverseMode = scene.traverseMode;

    keyFrame->dynamicLevels = scene.dynamicLevels;
    keyFrame->lockDynamicLevel = scene.lockDynamicLevel;

    keyFrame->minX = scene.voxelGrid.minX;
    keyFrame->minY = scene.voxelGrid.minY;
    keyFrame->minZ = scene.voxelGrid.minZ;
    keyFrame->maxX = scene.voxelGrid.maxX;
    keyFrame->maxY = scene.voxelGrid.maxY;
    keyFrame->maxZ = scene.voxelGrid.maxZ;
}

void Animation::applyNonCameraInfoToAllTheRest(Scene3D &scene)
{
    for(int i=currentKeyFrame; i<keyFrames.size(); i++)
    {
        KeyFrame &keyFrame = keyFrames[i];
        /*keyFrame.camCenterX = scene.camCenterX;
        keyFrame.camCenterY = scene.camCenterY;
        keyFrame.camCenterZ = scene.camCenterZ;
        keyFrame.camRotX = scene.camRotX;
        keyFrame.camRotY = scene.camRotY;
        keyFrame.camDistance = scene.camDistance;*/
        keyFrame.redChannel = scene.redChannel;
        keyFrame.greenChannel = scene.greenChannel;
        keyFrame.blueChannel = scene.blueChannel;
        keyFrame.redFactor = scene.redFactor;
        keyFrame.greenFactor = scene.greenFactor;
        keyFrame.blueFactor = scene.blueFactor;
        keyFrame.intensityFactor = scene.intensityFactor;
        keyFrame.deltaOffset = scene.deltaOffset;
        keyFrame.colorAlphaFactor = scene.colorAlphaFactor;
        keyFrame.colorRayStrengthFactor = scene.colorRayStrengthFactor;
        keyFrame.attenuationAlphaFactor = scene.attenuationAlphaFactor;

        for(int k=0; k<scene.shadingModes.size(); k++) {
            keyFrame.rayAttenuation[k] = scene.rayAttenuation[k];
        }

        keyFrame.shadingMode = scene.shadingMode;
        keyFrame.traverseMode = scene.traverseMode;

        keyFrame.dynamicLevels = scene.dynamicLevels;
        keyFrame.lockDynamicLevel = scene.lockDynamicLevel;

        keyFrame.minX = scene.voxelGrid.minX;
        keyFrame.minY = scene.voxelGrid.minY;
        keyFrame.minZ = scene.voxelGrid.minZ;
        keyFrame.maxX = scene.voxelGrid.maxX;
        keyFrame.maxY = scene.voxelGrid.maxY;
        keyFrame.maxZ = scene.voxelGrid.maxZ;
    }
}

int Animation::getKeyFrameCount() {
    return keyFrames.size();
}




Ray::Ray() {}

Ray::Ray(double dx, double dy, double dz) {
    direction.set(dx, dy, dz);
    direction.normalize();
}
Ray::Ray(double ox, double oy, double oz, double dx, double dy, double dz) {
    origin.set(ox, oy, oz);
    direction.set(dx, dy, dz);
    direction.normalize();
}

void Ray::update(MatrixStack mat) {
    mat.m.mulToLatter(origin, originEye, 1.0);
    mat.m.mulToLatter(direction, directionEye, 0.0);
}







Box::Box() {}

Box::Box(double s) {
    Box(s, s, s);
}

Box::Box(double w, double h, double d) {
    this->w = w;
    this->h = h;
    this->d = d;
    vertices.resize(8);
    verticesEye.resize(8);

    vertices[0].set(0.5, 0.5, 0.5);
    vertices[1].set(-0.5, 0.5, 0.5);
    vertices[2].set(-0.5, -0.5, 0.5);
    vertices[3].set(0.5, -0.5, 0.5);
    vertices[4].set(0.5, 0.5, -0.5);
    vertices[5].set(-0.5, 0.5, -0.5);
    vertices[6].set(-0.5, -0.5, -0.5);
    vertices[7].set(0.5, -0.5, -0.5);

}

void Box::update(MatrixStack &mat) {
    mat.push();
    mat.m.translate(pos.x, pos.y, pos.z);
    mat.m.scale(w, h, d);
    for(int i=0; i<vertices.size(); i++) {
        mat.m.mulToLatter(vertices[i], verticesEye[i], 1);
    }
    mat.pop();
}



VoxelGrid::VoxelGrid() { }

/*VoxelGrid(ImgWrapper img) {
updateVoxelGrid(img);
}*/

void VoxelGrid::setVoxel(int x, int y, int z, const Vec4d &value) {
    if(x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d) {
        voxels[x + y*w + z*w*h] = value;
    }
}
//void updateVoxelGrid(ImgWrapper img) {
void VoxelGrid::setSize(int w, int h, int d) {
    //this->img = img;
    voxels.resize(w*h*d);
    voxelCoordinates.resize(w*h*d);
    this->w = w;
    this->h = h;
    this->d = d;
    maxX = w;
    maxY = h;
    maxZ = d;


    xRange.set(-1, 1);
    yRange.set(-1, 1);
    zRange.set(-1, 1);

    if(w <= h && w <= d) {
        yRange.mul((double)h/w);
        zRange.mul((double)d/w);
    }
    else if(h <= w && h <= d) {
        xRange.mul((double)w/h);
        zRange.mul((double)d/h);
    }
    else if(d <= w && d <= h) {
        xRange.mul((double)w/d);
        yRange.mul((double)h/d);
    }

    Vec3d pos;
    for(int i=0; i<w; i++) {
        pos.x = map(i, 0, w-1, xRange.x, xRange.y);
        int xBorder = i == 0 || i == w-1 ? 1 : 0;
        for(int j=0; j<h; j++) {
            pos.y = map(j, 0, h-1, yRange.x, yRange.y);
            int yBorder = j == 0 || j == h-1 ? 1 : 0;
            for(int k=0; k<d; k++) {
                int zBorder = k == 0 || k == d-1 ? 1 : 0;
                pos.z = map(k, 0, d-1, zRange.x, zRange.y);
                voxelCoordinates[i + j*w + k*w*h] = Vec4d(pos.x, pos.y, pos.z, xBorder+yBorder+zBorder);
            }
        }
    }
}

// DDA traversal adapted from Amanatides and Woo, 1987
Vec4d VoxelGrid::traverseGridFast(Vec3d start, Vec3d end, Scene3D &scene)
{
    Vec3d dir = Vec3d::sub(end, start);
    dir.normalize();
    start = Vec3d::sub(start, pos);
    Vec3d currentPos(start.x+w/2-0.5, start.y+h/2-0.5, start.z+d/2-0.5);

    int nextVoxelX = dir.x > 0 ? 1 : -1;
    int nextVoxelY = dir.y > 0 ? 1 : -1;
    int nextVoxelZ = dir.z > 0 ? 1 : -1;

    int currentVoxelX = round(clamp(currentPos.x, 0, this->w-1));
    int currentVoxelY = round(clamp(currentPos.y, 0, this->h-1));
    int currentVoxelZ = round(clamp(currentPos.z, 0, this->d-1));

    int voxelsVisited = 0;

    double tMaxX = (currentVoxelX + 0.5*nextVoxelX - currentPos.x)/dir.x;
    double tMaxY = (currentVoxelY + 0.5*nextVoxelY - currentPos.y)/dir.y;
    double tMaxZ = (currentVoxelZ + 0.5*nextVoxelZ - currentPos.z)/dir.z;

    double tStepX = 1.0/dir.x*nextVoxelX;
    double tStepY = 1.0/dir.y*nextVoxelY;
    double tStepZ = 1.0/dir.z*nextVoxelZ;

    double rayStrength = 1.0;
    double rayAttenuation = scene.rayAttenuation[scene.shadingMode];
    rayAttenuation = rayAttenuation * rayAttenuation;
    double r = 0, g = 0, b = 0, a = 0;

    int tmpX = 1;
    int tmpY = tmpX*this->w;
    int tmpZ = tmpY*this->h;
    //int tmpT = tmpZ*scene.img.timePoint*scene.img.numZPlanes;
    int voxelIndex = currentVoxelX*tmpX + currentVoxelY*tmpY + currentVoxelZ*tmpZ;// + tmpT;

    int indexStepX = nextVoxelX*tmpX;
    int indexStepY = nextVoxelY*tmpY;
    int indexStepZ = nextVoxelZ*tmpZ;

    double redness = scene.redFactor * scene.intensityFactor;
    double greenness = scene.greenFactor * scene.intensityFactor;
    double blueness = scene.blueFactor * scene.intensityFactor;
    double dr = 0, dg = 0, db = 0;
    double maxR = 0, maxG = 0, maxB = 0, maxA = 0;
    bool border = false;

    if(((currentVoxelX == minX || currentVoxelX == maxX-1) ? 1 : 0) +
    ((currentVoxelY == minY || currentVoxelY == maxY-1) ? 1 : 0) +
    ((currentVoxelZ == minZ || currentVoxelZ == maxZ-1) ? 1 : 0) > 1) {
        border = true;
    }

    while(true)
    {
        voxelsVisited++;

        dr = voxels[voxelIndex].x * redness;
        dg = voxels[voxelIndex].y * greenness;
        db = voxels[voxelIndex].z * blueness;

        if(scene.shadingMode == 2) {
            r = dr;
            g = dg;
            b = db;
            break;
        }
        else if(scene.shadingMode == 1 && scene.maxIntOrAlpha) {
            r = dr > r ? dr : r;
            g = dg > g ? dg : g;
            b = db > b ? db : b;
            rayStrength -= rayAttenuation;
        }
        else if(scene.shadingMode == 1 && !scene.maxIntOrAlpha) {
            if(voxels[voxelIndex].w > a) {
                a = voxels[voxelIndex].w;
                r = dr;
                g = dg;
                b = db;
            }
            rayStrength -= rayAttenuation;
        }
        else {
            if(scene.maxIntOrAlpha) {
                maxR = dr > maxR ? dr : maxR;
                maxG = dg > maxG ? dg : maxG;
                maxB = db > maxB ? db : maxB;
            }
            else {
                if(voxels[voxelIndex].w > maxA) {
                    maxA = voxels[voxelIndex].w;
                    maxR = dr;
                    maxG = dg;
                    maxB = db;
                }
            }

            double q = (voxels[voxelIndex].w*scene.colorAlphaFactor + 1.0-scene.colorAlphaFactor) * (rayStrength*scene.colorRayStrengthFactor + 1.0-scene.colorRayStrengthFactor);

            r += dr * q;
            g += dg * q;
            b += db * q;
            rayStrength -= rayAttenuation * (voxels[voxelIndex].w*scene.attenuationAlphaFactor + 1.0-scene.attenuationAlphaFactor);
        }


        if(rayStrength < 0) break;

        if(tMaxX < tMaxY && tMaxX < tMaxZ) {
            currentVoxelX += nextVoxelX;
            if(currentVoxelX < minX || currentVoxelX > maxX-1) {
                currentVoxelX -= nextVoxelX;
                break;
            }
            tMaxX += tStepX;

            voxelIndex += indexStepX;
        }
        else if(tMaxY < tMaxZ) {
            currentVoxelY += nextVoxelY;
            if(currentVoxelY < minY || currentVoxelY > maxY-1) {
                voxelIndex -= indexStepY;
                break;
            }
            tMaxY += tStepY;
            voxelIndex += indexStepY;
        }
        else {
            currentVoxelZ += nextVoxelZ;
            if(currentVoxelZ < minZ || currentVoxelZ > maxZ-1) {
                voxelIndex -= indexStepZ;
                break;
            }
            tMaxZ += tStepZ;
            voxelIndex += indexStepZ;
        }
    }

    if(scene.shadingMode < 2)
    {
        if(scene.dynamicLevels) {
            double avgR = r/voxelsVisited, avgG = g/voxelsVisited, avgB = b/voxelsVisited;
            r = r*(1.0-scene.averageFactor) + avgR*scene.averageFactor;
            g = g*(1.0-scene.averageFactor) + avgG*scene.averageFactor;
            b = b*(1.0-scene.averageFactor) + avgB*scene.averageFactor;
            scene.previousMaxTmp = max(scene.previousMaxTmp, max(r, max(g, b)));
            scene.previousMinTmp = min(scene.previousMinTmp, min(r, min(g, b)));

            double fl = (1.0-scene.deltaOffset)*scene.previousMin + scene.deltaOffset*scene.previousMax;
            if(scene.previousMax > fl) {
                r = map(r, fl, scene.previousMax, 0, 1.0);
                g = map(g, fl, scene.previousMax, 0, 1.0);
                b = map(b, fl, scene.previousMax, 0, 1.0);
                r = r*(1.0-scene.mipFactor) + maxR*scene.mipFactor;
                g = g*(1.0-scene.mipFactor) + maxG*scene.mipFactor;
                b = b*(1.0-scene.mipFactor) + maxB*scene.mipFactor;
            }
            else {
                r = g = b = 0;
            }
        }
        else {
            double maxValue = max(1.0, max(r, max(g, b)));
            double minValue = scene.deltaOffset*min(r, min(g, b));
            if(maxValue > minValue) {
                r = map(r, minValue, maxValue, 0, 1.0);
                g = map(g, minValue, maxValue, 0, 1.0);
                b = map(b, minValue, maxValue, 0, 1.0);
            }
            else {
                r = g = b = 0;
            }
        }
    }

    if(((currentVoxelX == minX || currentVoxelX == maxX-1) ? 1 : 0) +
    ((currentVoxelY == minY || currentVoxelY == maxY-1) ? 1 : 0) +
    ((currentVoxelZ == minZ || currentVoxelZ == maxZ-1) ? 1 : 0) > 1) {
        border = true;
    }

    if(scene.showBorders && border) {
        double minValue = min(r, min(g, b));
        double maxValue = max(r, max(g, b));
        double lum = (minValue+maxValue+0.4)/2.0/1.5;
        r = r*lum + 0.15 * (1.0-lum);
        g = g*lum + 0.15 * (1.0-lum);
        b = b*lum + 0.15 * (1.0-lum);
    }


    return Vec4d(r, g, b, 1.0);
}





Vec4d VoxelGrid::traverseGrid(Vec3d start, Vec3d end, Scene3D &scene)
{
    Vec3d dir = Vec3d::sub(end, start);
    dir.normalize();
    Vec3d invDir(dir.x==0 ? 0 : 1.0/dir.x, dir.y==0 ? 0 : 1.0/dir.y, dir.z==0 ? 0 : 1.0/dir.z);
    start = Vec3d::sub(start, pos);
    Vec3d currentPos(start.x+w/2-0.5, start.y+h/2-0.5, start.z+d/2-0.5);

    int nextVoxelX = dir.x > 0 ? 1 : -1;
    int nextVoxelY = dir.y > 0 ? 1 : -1;
    int nextVoxelZ = dir.z > 0 ? 1 : -1;

    double nextVoxelHalfX = 0.5*nextVoxelX;
    double nextVoxelHalfY = 0.5*nextVoxelY;
    double nextVoxelHalfZ = 0.5*nextVoxelZ;

    int currentVoxelX = round(clamp(currentPos.x, 0, this->w-1));
    int currentVoxelY = round(clamp(currentPos.y, 0, this->h-1));
    int currentVoxelZ = round(clamp(currentPos.z, 0, this->d-1));

    double t = 0, tx = 0, ty = 0, tz = 0;

    double rayStrength = 1.0;
    double r = 0, g = 0, b = 0, a;
    int voxelsVisited = 0;

    int tmpX = 1;
    int tmpY = tmpX*this->w;
    int tmpZ = tmpY*this->h;
    //int tmpT = scene.img.timePoint*scene.img.numZPlanes*tmpZ;
    int voxelIndex = currentVoxelX*tmpX + currentVoxelY*tmpY + currentVoxelZ*tmpZ;// + tmpT;

    int indexStepX = nextVoxelX*tmpX;
    int indexStepY = nextVoxelY*tmpY;
    int indexStepZ = nextVoxelZ*tmpZ;

    double redness = scene.redFactor * scene.intensityFactor;
    double greenness = scene.greenFactor * scene.intensityFactor;
    double blueness = scene.blueFactor * scene.intensityFactor;
    double dr = 0, dg = 0, db = 0;

    while(true) {
        voxelsVisited++;
        tx = (currentVoxelX + nextVoxelHalfX - currentPos.x)*invDir.x;
        ty = (currentVoxelY + nextVoxelHalfY - currentPos.y)*invDir.y;
        tz = (currentVoxelZ + nextVoxelHalfZ - currentPos.z)*invDir.z;

        if(tx < ty && tx < tz) {
            t = tx;
        }
        else if (ty < tz) {
            t = ty;
        }
        else {
            t = tz;
        }

        /*dr = scene.redChannel < 0 || scene.redChannel + voxelIndex >= img.bytes.size() ? 0 : redness * (img.bytes[scene.redChannel + voxelIndex] & 255);
        dg = scene.greenChannel < 0 || scene.greenChannel + voxelIndex >= img.bytes.size() ? 0 : greenness * (img.bytes[scene.greenChannel + voxelIndex] & 255);
        db = scene.blueChannel < 0 || scene.blueChannel + voxelIndex >= img.bytes.size() ? 0 : blueness * (img.bytes[scene.blueChannel + voxelIndex] & 255);*/

        if(scene.redChannel == 0) dr = voxels[voxelIndex].x * redness;
        else if(scene.redChannel == 1) dr = voxels[voxelIndex].y * redness;
        else if(scene.redChannel == 2) dr = voxels[voxelIndex].z * redness;
        else dr = 0;

        if(scene.greenChannel == 0) dg = voxels[voxelIndex].x * greenness;
        else if(scene.greenChannel == 1) dg = voxels[voxelIndex].y * greenness;
        else if(scene.greenChannel == 2) dg = voxels[voxelIndex].z * greenness;
        else dg = 0;

        if(scene.blueChannel == 0) db = voxels[voxelIndex].x * blueness;
        else if(scene.blueChannel == 1) db = voxels[voxelIndex].y * blueness;
        else if(scene.blueChannel == 2) db = voxels[voxelIndex].z * blueness;
        else db = 0;


        double minValue = min(dr, min(dg, db));
        double maxValue = max(dr, max(dg, db));
        a = (minValue+maxValue)/2.0;
        //totalA += a;

        if(scene.shadingMode == 0) {
            r += rayStrength*dr*a*t;
            g += rayStrength*dg*a*t;
            b += rayStrength*db*a*t;
            rayStrength -= a * scene.rayAttenuation[scene.shadingMode]*t;
        }
        else if(scene.shadingMode == 1) {
            r += rayStrength*dr*t;
            g += rayStrength*dg*t;
            b += rayStrength*db*t;
            rayStrength -= a * scene.rayAttenuation[scene.shadingMode]*t;
        }
        else if(scene.shadingMode == 2) {
            r += dr*t;
            g += dg*t;
            b += db*t;
            rayStrength -= scene.rayAttenuation[scene.shadingMode]*scene.rayAttenuation[scene.shadingMode]*t;
        }
        else if(scene.shadingMode == 3) {
            r = dr > r ? dr : r;
            g = dg > g ? dg : g;
            b = db > b ? db : b;
            rayStrength -= scene.rayAttenuation[scene.shadingMode]*scene.rayAttenuation[scene.shadingMode];
        }
        else {
            r = dr;
            g = dg;
            b = db;
            break;
        }

        if(rayStrength < 0) break;


        if(tx < ty && tx < tz) {
            currentVoxelX += nextVoxelX;
            voxelIndex += indexStepX;
            if(currentVoxelX < 0 || currentVoxelX > this->w-1) {
                break;
            }
        }
        else if (ty < tz) {
            currentVoxelY += nextVoxelY;
            voxelIndex += indexStepY;
            if(currentVoxelY < 0 || currentVoxelY > this->h-1) {
                break;
            }
        }
        else {
            currentVoxelZ += nextVoxelZ;
            voxelIndex += indexStepZ;
            if(currentVoxelZ < 0 || currentVoxelZ > this->d-1) {
                break;
            }
        }

        currentPos.x += t*dir.x;
        currentPos.y += t*dir.y;
        currentPos.z += t*dir.z;
    }

    if(scene.shadingMode == 2) {
        r /= voxelsVisited;
        g /= voxelsVisited;
        b /= voxelsVisited;
    }

    Vec4d pixelColor;

    if(scene.shadingMode < 3)
    {
        double lightnessScale = 0;

        if(scene.dynamicLevels) {
            scene.previousMaxTmp = max(scene.previousMaxTmp, max(r, max(g, b)));
            lightnessScale = 1.0/scene.previousMax;
        }
        else {
            lightnessScale = 1.0/max(1.0, max(r, max(g, b)));
        }

        if(scene.shadingMode < 2) {
            pixelColor.set(r*lightnessScale, g*lightnessScale, b*lightnessScale, max(0.0, 1.0-rayStrength));
        }
        else {
            pixelColor.set(r*lightnessScale, g*lightnessScale, b*lightnessScale, 1.0);
        }
    }
    else {
        pixelColor.set(r, g, b, 1.0);
    }

    return pixelColor;

}

void min2(std::vector<double> &v, float a) {
    if(v[0] > a) {
        v[1] = v[0];
        v[0] = a;
    }
    else if(v[1] > a) {
        v[1] = a;
    }
}

std::vector<double> intersectRayAAB(Ray &ray, VoxelGrid &grid)
{

    Vec3d S = Vec3d::sub(ray.origin, grid.pos);
    /*double hw = (grid.maxX-grid.minX)*0.5, hh = (grid.maxY-grid.minY)*0.5, hd = (grid.maxZ-grid.minZ)*0.5;
    S.x += hw - grid.w*0.5;
    S.y += hh - grid.h*0.5;
    S.z += hd - grid.d*0.5;*/
    Vec3d D = ray.direction;
    Vec3d P;
    double x1 = floor(grid.minX) - grid.w*0.5;
    double x2 = ceil(grid.maxX) - grid.w*0.5;
    double y1 = floor(grid.minY) - grid.h*0.5;
    double y2 = ceil(grid.maxY) - grid.h*0.5;
    double z1 = floor(grid.minZ) - grid.d*0.5;
    double z2 = ceil(grid.maxZ) - grid.d*0.5;

    int numIntersections = 0;

    std::vector<double> t = {3.4e38f, 3.4e38f};

    double t1 = (x1-S.x)/D.x;
    P.set(x1, S.y + t1*D.y, S.z + t1*D.z);
    if(t1 > 0 && P.y < y2 && P.y >= y1 && P.z < z2 && P.z >= z1) {
        min2(t, t1);
    }

    double t2 = (x2-S.x)/D.x;
    P.set(x2, S.y + t2*D.y, S.z + t2*D.z);
    if(t2 > 0 && P.y < y2 && P.y >= y1 && P.z < z2 && P.z >= z1) {
        min2(t, t2);
    }

    double t3 = (y1-S.y)/D.y;
    P.set(S.x + t3*D.x, y1, S.z + t3*D.z);
    if(t3 > 0 && P.x < x2 && P.x >= x1 && P.z < z2 && P.z >= z1) {
        min2(t, t3);
    }

    double t4 = (y2-S.y)/D.y;
    P.set(S.x + t4*D.x, y2, S.z + t4*D.z);
    if(t4 > 0 && P.x < x2 && P.x >= x1 && P.z < z2 && P.z >= z1) {
        min2(t, t4);
    }

    double t5 = (z1-S.z)/D.z;
    P.set(S.x + t5*D.x, S.y + t5*D.y, z1);
    if(t5 > 0 && P.x < x2 && P.x >= x1 && P.y < y2 && P.y >= y1) {
        min2(t, t5);
    }

    double t6 = (z2-S.z)/D.z;
    P.set(S.x + t6*D.x, S.y + t6*D.y, z2);
    if(t6 > 0 && P.x < x2 && P.x >= x1 && P.y < y2 && P.y >= y1) {
        min2(t, t6);
    }

    if(t[0] < 3e38f) numIntersections++;
    if(t[1] < 3e38f) numIntersections++;

    std::vector<double> ret = {(double)numIntersections, t[0], t[1]};
    return ret;
};





void Scene3D::startSaving() {
    saveFrameCounter = 0;
    baseFilename = "frame ";
    //baseFilename += " "+String.format("%d-%d-%d %01d-%01d-%01d",day(), month(), year(), hour(), minute(), second());
    baseFilename += getTimeString();

    saveToFile = true;
}

void Scene3D::stopSaving() {
    saveToFile = false;
}

void Scene3D::saveScene(bool sequence)
{
    std::string filename = "";
    if(sequence) {
        //filename = baseFilename + String.format("_%06d", saveFrameCounter);
        filename = baseFilename + std::to_string(saveFrameCounter);
    }
    else {
        //filename = "image " + String.format("%d-%d-%d %01d-%01d-%01d",day(), month(), year(), hour(), minute(), second());
        filename = "image_";
        filename += getTimeString();
    }

    /*File f = new File("C:/Users/lassi/Dropbox/sketches/Stats");
    if(!f.exists()) {
    f = new File("C:/Users/laanpalm/Dropbox/sketches/Stats");
}
if(!f.exists()) {
f = new File(System.getProperty("user.dir"));
}*/

rayTracedSceneCurrent.saveToFile("volray/output/"+filename+".png");

saveFrameCounter++;
}

void Scene3D::setUseGpu(bool useGpu) {
    this->useGpu = useGpu;
    if(useGpu) {
        if(!voxelGridTexture) {
            voxelGridTexture = new Texture();
            voxelGridTexture->createArray(voxelGrid.w, voxelGrid.h, voxelGrid.d, Texture::PixelType::RGBA);
            minMaxVoxelValuesTexture = new Texture();
            minMaxVoxelValuesTexture->create(2, 1, Texture::PixelType::UINT);
            quad.create(1, 1, false);
        }
        if(voxelGridTexture->w != voxelGrid.w || voxelGridTexture->h != voxelGrid.h || voxelGridTexture->textureArraySize != voxelGrid.d) {
            voxelGridTexture->createArray(voxelGrid.w, voxelGrid.h, voxelGrid.d, Texture::PixelType::RGBA);
        }
        if(!voxelTracerShader.readyToUse) {
            reloadShaders();
        }
    }
}

void Scene3D::reloadShaders() {
    voxelTracerShader.create("data/glsl/texture.vert", "data/glsl/voxelRayTracer.frag");
}

void Scene3D::setVoxelGridSize(int w, int h, int d) {
    voxelGrid.setSize(w, h, d);
    if(useGpu && voxelGridTexture) {
        if(voxelGridTexture->w != voxelGrid.w || voxelGridTexture->h != voxelGrid.h || voxelGridTexture->textureArraySize != voxelGrid.d) {
            voxelGridTexture->createArray(voxelGrid.w, voxelGrid.h, voxelGrid.d, Texture::PixelType::RGBA);
        }
    }
}

//void update(ImgWrapper img) {
void Scene3D::update() {
    //this->img = img;
    //voxelGrid.updateVoxelGrid(img);
    redFactor = 1.0;
    greenFactor = 1.0;
    blueFactor = 1.0;
    intensityFactor = 1.0;
    deltaOffset = 0;
    rayAttenuation[0] = 0.4;
    rayAttenuation[1] = 0.15;
    rayAttenuation[2] = 0.0;
    shadingMode = 0;
    traverseMode = 0;
    previewRenderIndex = 1;
    finalRenderIndex = 3;
    /*redChannel = img.channel;
    greenChannel = img.channel;
    blueChannel = img.channel;*/
    dynamicLevels = true;
    previousMax = 1.0;
    previousMaxTmp = 0.0;
    previousMin = 0.0;
    previousMinTmp = 0.0;
    rayTracingDone = false;
}

// probably the voxel data should be passed here
Scene3D::Scene3D(int w, int h) {
    this->w = w;
    this->h = h;
    //super(name);
    //this->panelBuf = panelBuf;
    //rayTracedScene = new PImage[5];
    resetCanvases();
    //rayTracedSceneCurrent = rayTracedScene[3];
}

void Scene3D::resetCanvases() {
    if(rayTracedSceneCurrent.w !=w/pixelization || rayTracedSceneCurrent.h != h/pixelization) {
        rayTracedSceneCurrent.create(w/pixelization, h/pixelization);
    }
    else {
        rayTracedSceneCurrent.clearGl();
    }
    /*rayTracedScene[0] = new PImage(panelBuf.w/8, panelBuf.h/8, ARGB);
    rayTracedScene[1] = new PImage(panelBuf.w/4, panelBuf.h/4, ARGB);
    rayTracedScene[2] = new PImage(panelBuf.w/2, panelBuf.h/2, ARGB);
    rayTracedScene[3] = new PImage(panelBuf.w, panelBuf.h, ARGB);

    rayTracedScene[4] = new PImage(w, h, ARGB);*/
    rayTracingDone = false;
}

void Scene3D::setReady(bool ready)  {
    rayTracingDone = ready;
}
bool Scene3D::isReady() {
    return rayTracingDone;
}

void Scene3D::newFrame(bool insert) {
    anim.applyFromScene(anim.getKeyFrameCount(), insert, *this);
    if(!insert) {
        anim.currentKeyFrame = anim.getKeyFrameCount()-1;
    }
}

void Scene3D::applyFrame() {
    anim.applyFromScene(anim.currentKeyFrame, false, *this);
}


void Scene3D::updateCamera(double dx, double dy, double dz, int mode)
{
    if(mode == 0) {
        camRotY += dx*0.01;
        camRotX -= dy*0.01;
        camRotX = clamp(camRotX, -PI*0.5, PI*0.5);
    }
    else if(mode == 1) {
        camDistance *= dx > 0 ? (1.0+dx*0.01) : 1.0/(1.0-dx*0.01);
    }
    else if(mode == 2) {
        if(cameraMode == 0) {
            double sdx = dx*camDistance*0.005;
            double sdy = dy*camDistance*0.005;
            double sdz = -dz*camDistance*0.005;
            camCenterX += cos(camRotY)*sdx + sin(camRotY)*sdz;
            camCenterZ += -cos(camRotY)*sdz + sin(camRotY)*sdx;
            camCenterY += sdy;
        }
        else {
            mat.push();
            if(flipMode == 1) mat.m.rotateX(-PI/2);
            if(flipMode == 2) mat.m.rotateX(PI/2);
            if(flipMode == 3) mat.m.rotateZ(-PI/2);
            if(flipMode == 4) mat.m.rotateZ(PI/2);
            Matrix4d inv(mat.m);
            inv.invert();
            mat.pop();
            double speed = dz * (keyDot && keyComma ? 60 : (keyDot ? 2 : (keyComma ? 20 : 10)));
            Vec3d dir(0, 0, speed);
            inv.mulDirection(dir);
            camCenterX += dir.x;
            camCenterY += dir.y;
            camCenterZ += dir.z;
        }
    }

    rayTracingDone = false;
}

void Scene3D::updateView() {
    //for(Panel panel : panels) {
    //if(panel.isIn(mouseX, mouseY)) {
    //Scene3D scene = panel.scene;
    //if(scene == this) {
    double dx = 0, dy = 0, dz = 0;
    double speed = 20;//e.lAltDown ? 5.0 : (e.lControlDown ? 10 : 20.0);
    if(keyLeft) { dx += speed; }
    if(keyRight) { dx -= speed; }
    if(keyUp) { dz += speed; }
    if(keyDown) { dz -= speed; }
    if(keyDot) { dy += speed; }
    if(keyComma) { dy -= speed; }
    if(dx != 0 || dy != 0 || dz != 0) {
        updateCamera(dx, dy, dz, 2);
    }

    /*if(sceneSynchronization > 0) {
    for(Panel otherPanel : panels) {
    if(otherPanel.scene == this) continue;
    otherPanel.scene.camCenterX = scene.camCenterX;
    otherPanel.scene.camCenterY = scene.camCenterY;
    otherPanel.scene.camCenterZ = scene.camCenterZ;
    otherPanel.scene.camRotX = scene.camRotX;
    otherPanel.scene.camRotY = scene.camRotY;
    otherPanel.scene.camDistance = scene.camDistance;
    otherPanel.scene.rayTracingDone = false;
}
}*/
//}
//}
//}
}

static void render(int threadNumber, Scene3D *scene3D) {
    Ray theRay;

    for(int j=0; j<scene3D->rayTracedSceneCurrent.h; j++) {
        for(int i=threadNumber; i<scene3D->rayTracedSceneCurrent.w; i+=scene3D->numRenderingThreads) {
            double cameraDistance = 1.0/tan(scene3D->verticalFOV*0.5*PI/180.0);
            //theRay.origin.set(0, 0, cameraDistance);

            //          theRay.direction.set(map(i+rayTracedSceneCurrent.w*xOffset*xOffsetFactor/camDistance*cameraDistance, 0, rayTracedSceneCurrent.w-1, -aspect, aspect),
            //                                             map(j, 0, rayTracedSceneCurrent.h-1, -1, 1), -cameraDistance);
            theRay.direction.set(map(i+scene3D->rayTracedSceneCurrent.w/scene3D->camDistance*cameraDistance, 0, scene3D->rayTracedSceneCurrent.w-1, -scene3D->aspectRatio, scene3D->aspectRatio),
            map(j, 0, scene3D->rayTracedSceneCurrent.h-1, -1, 1), -cameraDistance);
            theRay.direction.normalize();

            //theRay.origin.set(-xOffset, 0, 0);
            theRay.origin.set(0, 0, 0);
            scene3D->inverseCameraMatrix.mul(theRay.origin, 1.0);
            scene3D->inverseCameraMatrix.mulDirection(theRay.direction);

            Vec4d rayColor(0, 0, 0, 0);

            std::vector<double> ret = intersectRayAAB(theRay, scene3D->voxelGrid);

            if(ret[0] > 0) {
                Vec3d start, end;
                Vec3d o = theRay.origin, d = theRay.direction;

                if(ret[0] == 1) {
                    start = theRay.origin;
                    end.set(o.x + d.x*ret[1], o.y + d.y*ret[1], o.z + d.z*ret[1]);
                }

                if(ret[0] > 1) {
                    start.set(o.x + d.x*ret[1], o.y + d.y*ret[1], o.z + d.z*ret[1]);
                    end.set(o.x + d.x*ret[2], o.y + d.y*ret[2], o.z + d.z*ret[2]);
                }
                if(scene3D->traverseMode == 0) {
                    rayColor = scene3D->voxelGrid.traverseGridFast(start, end, *scene3D);
                }
                else {
                    rayColor = scene3D->voxelGrid.traverseGrid(start, end, *scene3D);
                }
            }

            //rayTracedSceneCurrent.pixels[i+j*rayTracedSceneCurrent.w] = rayColor;
            scene3D->rayTracedSceneCurrent.setPixel(i, j, rayColor);
        }
    }
}

void Scene3D::renderWithGpu(double time) {

    bool isLastFrame = false;
    if(useAnimation && animationRunning) {
        anim.applyToScene(*this);
        isLastFrame = anim.isLastFrame(loopMode);
        rayTracingDone = false;
    }

    if(!rayTracingDone)
    {
        long timeStartRendering = time;

        mat.m.setIdentity();

        mat.m.setCamera(camDistance, camRotX, camRotY);
        mat.m.translate(camCenterX, camCenterY, camCenterZ);
        if(flipMode == 1) mat.m.rotateX(PI/2);
        if(flipMode == 2) mat.m.rotateX(-PI/2);
        if(flipMode == 3) mat.m.rotateZ(PI/2);
        if(flipMode == 4) mat.m.rotateZ(-PI/2);
        mat.m.scale(100);

        inverseCameraMatrix = mat.m;
        inverseCameraMatrix.invert();

        aspectRatio = (double)rayTracedSceneCurrent.w/rayTracedSceneCurrent.h;

        minMaxVoxelValuesTexture->uintPixels[0] = -1;
        minMaxVoxelValuesTexture->uintPixels[1] = 0;
        minMaxVoxelValuesTexture->applyPixels();

        quad.setSize(w, h);

        voxelTracerShader.activate();
        voxelTracerShader.setUniform2f("screenSize", w, h);
        voxelTracerShader.setUniform1f("aspectRatio", aspectRatio);
        voxelTracerShader.setUniformMatrix4fv("inverseCameraMatrix", 1, inverseCameraMatrix.k);
        voxelTracerShader.setUniform1f("cameraDistance",  1.0/tan(verticalFOV*0.5*PI/180.0));
        voxelTracerShader.setUniform1f("cameraDistanceFactorX", camDistance);

        voxelTracerShader.setUniform3f("gridPos", voxelGrid.pos);
        voxelTracerShader.setUniform1i("gridW", (int)voxelGrid.w);
        voxelTracerShader.setUniform1i("gridH", (int)voxelGrid.h);
        voxelTracerShader.setUniform1i("gridD", (int)voxelGrid.d);
        voxelTracerShader.setUniform1i("gridMinX", (int)voxelGrid.minX);
        voxelTracerShader.setUniform1i("gridMinY", (int)voxelGrid.minY);
        voxelTracerShader.setUniform1i("gridMinZ", (int)voxelGrid.minZ);
        voxelTracerShader.setUniform1i("gridMaxX", (int)voxelGrid.maxX);
        voxelTracerShader.setUniform1i("gridMaxY", (int)voxelGrid.maxY);
        voxelTracerShader.setUniform1i("gridMaxZ", (int)voxelGrid.maxZ);

        voxelTracerShader.setUniform1f("rayAttenuation", rayAttenuation[shadingMode]);

        voxelTracerShader.setUniform1f("deltaOffset", deltaOffset);
        voxelTracerShader.setUniform1f("previousMax", previousMax*0.0001);
        voxelTracerShader.setUniform1f("previousMin", previousMin*0.0001);
        voxelTracerShader.setUniform4f("colorFactors", Vec4d(redFactor, greenFactor, blueFactor, intensityFactor));
        voxelTracerShader.setUniform1i("shadingMode", shadingMode);
        voxelTracerShader.setUniform1i("dynamicLevels", dynamicLevels);
        voxelTracerShader.setUniform1i("maxIntOrAlpha", maxIntOrAlpha);
        voxelTracerShader.setUniform1i("showBorders", showBorders);
        voxelTracerShader.setUniform1f("mipFactor", mipFactor);
        voxelTracerShader.setUniform1f("xOffsetFactor", xOffsetFactor);
        voxelTracerShader.setUniform1f("attenuationAlphaFactor", attenuationAlphaFactor);
        voxelTracerShader.setUniform1f("colorAlphaFactor", colorAlphaFactor);
        voxelTracerShader.setUniform1f("colorRayStrengthFactor", colorRayStrengthFactor);


        voxelGridTexture->activateImageStorage(voxelTracerShader, "voxels", 0);
        minMaxVoxelValuesTexture->activateImageStorage(voxelTracerShader, "minMaxVoxelValuesImage", 1);
        quad.render();
        minMaxVoxelValuesTexture->inactivate(1);
        voxelGridTexture->inactivate(0);
        voxelTracerShader.deActivate();

        renderingTime = (time-timeStartRendering)*0.001;
        //rayTracedSceneCurrent.applyPixels();

        if(!lockDynamicLevel) {
            minMaxVoxelValuesTexture->loadGlPixels();
            previousMin = minMaxVoxelValuesTexture->uintPixels[0];
            previousMax = minMaxVoxelValuesTexture->uintPixels[1];
            //printf("%f ... %f\n", previousMin, previousMax);
        }

        if(useAnimation) {
                /*if(saveToFile) {
                saveScene(true);
            }*/
            if(animationRunning) {
                if(loopMode != 2 && isLastFrame) {
                    animationRunning = false;
                    //stopSaving();
                }
                else {
                    anim.deltaFrame(animationStep, loopMode);
                }
            }
        }
    }
}

void Scene3D::update(double time) {

    if(useGpu) {
        renderWithGpu(time);
        return;
    }

    bool isLastFrame = false;
    if(useAnimation && animationRunning) {
        anim.applyToScene(*this);
        isLastFrame = anim.isLastFrame(loopMode);
        rayTracingDone = false;
    }

    if(!rayTracingDone)
    {
        long timeStartRendering = time;

        //double xOffset = 0;
        mat.m.setIdentity();
        /*for(int i=0; i<4; i++) {
        if(panels[i].scene == this) {
        xOffset = stereoDistance * (i%2 == 1 ? 100.0 : -100.0);
        }
        }*/
        //mat.m.translate(xOffset, 0, 0);
        mat.m.setCamera(camDistance, camRotX, camRotY);
        mat.m.translate(camCenterX, camCenterY, camCenterZ);
        if(flipMode == 1) mat.m.rotateX(PI/2);
        if(flipMode == 2) mat.m.rotateX(-PI/2);
        if(flipMode == 3) mat.m.rotateZ(PI/2);
        if(flipMode == 4) mat.m.rotateZ(-PI/2);
        mat.m.scale(100);

        //Matrix4d inv(mat.m);
        inverseCameraMatrix = mat.m;
        inverseCameraMatrix.invert();

        //bool showPreview = dragged || keyLeft || keyRight || keyUp || keyDown;
        //rayTracedSceneCurrent = rayTracedScene[showPreview || stayInPreview ? previewRenderIndex : finalRenderIndex];

        //double aspect = (double)panelBuf.width/panelBuf.h;
        aspectRatio = (double)rayTracedSceneCurrent.w/rayTracedSceneCurrent.h;

        previousMaxTmp = 0;
        previousMinTmp = 1e38;

        for(int i=0; i<numRenderingThreads; i++) {
            renderingThreads.push_back(std::thread(render, i, this));
        }
        for(int i=0; i<renderingThreads.size(); i++) {
            renderingThreads[i].join();
        }
        renderingThreads.clear();

        renderingTime = (time-timeStartRendering)*0.001;
        rayTracedSceneCurrent.applyPixels();

        /*if(!showPreview && previousMax == previousMaxTmp) {
        rayTracingDone = true;
        }*/
        if(!lockDynamicLevel) {
            previousMax = previousMaxTmp;
            previousMin = previousMinTmp;
        }

        if(useAnimation) {
            /*if(saveToFile) {
            saveScene(true);
            }*/
            if(animationRunning) {
                if(loopMode != 2 && isLastFrame) {
                    animationRunning = false;
                    //stopSaving();
                }
                else {
                    anim.deltaFrame(animationStep, loopMode);
                }
            }
        }
    }
}

void Scene3D::draw(int x, int y, int w, int h) {
    //    image(rayTracedSceneCurrent, x, y, fitW(rayTracedSceneCurrent.w, rayTracedSceneCurrent.h, w, h),
    //                                       fitH(rayTracedSceneCurrent.w, rayTracedSceneCurrent.h, w, h));
    //image(rayTracedSceneCurrent, x, y, w, h);
    rayTracedSceneCurrent.render(x, y, w, h);
}

std::string dec(double d) {
    return format("%.2f", d);
}

std::string dec(double d, int n) {
    return format("%."+ std::to_string(n) + "f", d);
}

void Scene3D::printInfo(TextGl &textRenderer, int x, int y, int w, int h, int verbality, bool left) {
    if(!showInfo) return;
    //if(showTechnicalInfo) {
    int line = y - 10, column = x + (left ? 10 : w-10);
    textRenderer.print(shadingModes[shadingMode], column, line+=20, 10);
    if(flipMode > 0) {
        textRenderer.print(flipModes[flipMode], column, line+=20, 10);
    }
    /*if(sceneSynchronization > 0) {
    textRenderer.print(sceneSynchronization == 2 ? "Sync scenes" : "Sync cameras", column, line+=20, 10);
    if(abs(stereoDistance) > 0.00001)
    textRenderer.print("Stereo dist. "+dec(stereoDistance, 2), column, line+=20, 10);
}*/
textRenderer.print("x off. "+dec(xOffsetFactor, 2), column, line+=20, 10);
textRenderer.print("FOV "+dec(verticalFOV, 2), column, line+=20, 10);
textRenderer.print(std::to_string(rayTracedSceneCurrent.w)+"x"+std::to_string(rayTracedSceneCurrent.h), column, line+=20, 10);
textRenderer.print(dynamicLevels ? "dynamic levels" : "fixed levels", column, line+=20, 10);
textRenderer.print(traverseMode == 0 ? "non-weighted" : "weighted", column, line+=20, 10);
textRenderer.print(maxIntOrAlpha ? "max int." : "max alpha", column, line+=20, 10);
textRenderer.print("col. a.f. "+dec(colorAlphaFactor, 2), column, line+=20, 10);
textRenderer.print("col. r.f. "+dec(colorRayStrengthFactor, 2), column, line+=20, 10);
textRenderer.print("att. a.f. "+dec(attenuationAlphaFactor, 2), column, line+=20, 10);
textRenderer.print("att. "+dec(rayAttenuation[shadingMode], 2), column, line+=20, 10);
textRenderer.print("int. "+dec(intensityFactor, 2), column, line+=20, 10);
textRenderer.print("off. "+dec(deltaOffset, 2), column, line+=20, 10);
if(redChannel != -1)
textRenderer.print("r "+std::to_string(redChannel+1)+", "+dec(redFactor, 2), column, line+=20, 10);
if(greenChannel != -1)
textRenderer.print("g "+std::to_string(greenChannel+1)+", "+dec(greenFactor, 2), column, line+=20, 10);
if(blueChannel != -1)
textRenderer.print("b "+std::to_string(blueChannel+1)+", "+dec(blueFactor, 2), column, line+=20, 10);
textRenderer.print("MIP  "+dec(mipFactor, 2), column, line+=20, 10);
textRenderer.print("avg. "+dec(averageFactor, 2), column, line+=20, 10);
textRenderer.print("threads "+std::to_string(numRenderingThreads), column, line+=20, 10);
textRenderer.print("img. qual. "+std::to_string(pixelization), column, line+=20, 10);

if(useAnimation) {
    line += 8;
    if(anim.getKeyFrameCount() > 0) {
        textRenderer.print("keyframe "+std::to_string(anim.currentKeyFrame+1)+"/"+std::to_string(anim.getKeyFrameCount()), column, line+=20, 10);
        textRenderer.print("subframe "+std::to_string(anim.currentSubFrame+1)+"/"+std::to_string(anim.numSubFrames), column, line+=20, 10);
    }
    else {
        textRenderer.print("keyframe 0/0", column, line+=20, 10);
        textRenderer.print("subframe 0/0", column, line+=20, 10);
    }
    textRenderer.print("framestep "+std::to_string(animationStep), column, line+=20, 10);
    textRenderer.print(loopModes[loopMode], column, line+=20, 10);
    if(saveToFile)
    textRenderer.print("save to file", column, line+=20, 10);
}
//}
}


void Scene3D::onKeyUp(Events &e) {
    if(e.sdlKeyCode == SDLK_LEFT) {
        keyLeft = false;
    }
    if(e.sdlKeyCode == SDLK_RIGHT) {
        keyRight = false;
    }
    if(e.sdlKeyCode == SDLK_UP) {
        keyUp = false;
    }
    if(e.sdlKeyCode == SDLK_DOWN) {
        keyDown = false;
    }
    if(e.sdlKeyCode == SDLK_COMMA) {
        keyComma = false;
    }
    if(e.sdlKeyCode == SDLK_PERIOD) {
        keyDot = false;
    }
}

void Scene3D::onKeyDown(Events &e) {
    if(e.sdlKeyCode == SDLK_LEFT) {
        keyLeft = true;
    }
    if(e.sdlKeyCode == SDLK_RIGHT) {
        keyRight = true;
    }
    if(e.sdlKeyCode == SDLK_UP) {
        keyUp = true;
    }
    if(e.sdlKeyCode == SDLK_DOWN) {
        keyDown = true;
    }
    if(e.sdlKeyCode == SDLK_COMMA) {
        keyComma = true;
    }
    if(e.sdlKeyCode == SDLK_PERIOD) {
        keyDot = true;
    }

    /*if(e.sdlKeyCode == SDLK_1 && !e.lShiftDown) {
    redChannel++;
    if(redChannel >= img.numChannels) redChannel = -1;
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_1 && e.lShiftDown) {
redChannel--;
if(redChannel < -1) redChannel = img.numChannels-1;
rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_2 && !e.lShiftDown) {
greenChannel++;
if(greenChannel >= img.numChannels) greenChannel = -1;
rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_2 && e.lShiftDown) {
greenChannel--;
if(greenChannel < -1) greenChannel = img.numChannels-1;
rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_3 && !e.lShiftDown) {
blueChannel++;
if(blueChannel >= img.numChannels) blueChannel = -1;
rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_3 && e.lShiftDown) {
blueChannel--;
if(blueChannel < -1) blueChannel = img.numChannels-1;
rayTracingDone = false;
}*/


double adjustAmount = e.lControlDown ? 0.0001 : 0.01;

if(e.sdlKeyCode == SDLK_i) {
    showInfo = !showInfo;
}
if(e.sdlKeyCode == SDLK_u) {
    maxIntOrAlpha = !maxIntOrAlpha;
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_q && !e.lShiftDown && e.numModifiersDown == 0) {
    rayAttenuation[shadingMode] = max(rayAttenuation[shadingMode]-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_q && e.lShiftDown && e.numModifiersDown == 1) {
    rayAttenuation[shadingMode] = min(rayAttenuation[shadingMode]+adjustAmount, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_q && e.lShiftDown && e.lControlDown && e.numModifiersDown == 2) {
    stayInPreview = !stayInPreview;
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_0 && !e.lShiftDown) {
    previewRenderIndex = cycleValue(previewRenderIndex+1, finalRenderIndex);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_0 && e.lShiftDown) {
    previewRenderIndex = cycleValue(previewRenderIndex-1, finalRenderIndex);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_QUESTION && !e.lShiftDown) {
    finalRenderIndex = cycleValue(finalRenderIndex+1, previewRenderIndex, rayTracedScene.size()-1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_QUESTION && e.lShiftDown) {
    finalRenderIndex = cycleValue(finalRenderIndex-1, previewRenderIndex, rayTracedScene.size()-1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_PLUS) {
    shadingMode = cycleValue(shadingMode+1, shadingModes.size()-1);
    previousMin = 1e38;
    previousMax = 0.0;
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_w && !e.lShiftDown) {
    dynamicLevels = !dynamicLevels;
    previousMin = 1e38;
    previousMax = 0.0;
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_w && e.lShiftDown) {
    lockDynamicLevel = !lockDynamicLevel;
    rayTracingDone = false;
}
/*if(e.sdlKeyCode == SDLK_e && !e.lShiftDown) {
greenChannel = redChannel;
blueChannel = redChannel;
rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_e && e.lShiftDown) {
greenChannel = redChannel + img.numChannels;
blueChannel = redChannel + img.numChannels * 2;
rayTracingDone = false;
}*/
if(e.sdlKeyCode == SDLK_r && !e.lShiftDown) {
    traverseMode = 1-traverseMode;
    previousMin = 1e38;
    previousMax = 0.0;
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_r && e.lShiftDown) {
    showBorders = !showBorders;
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_t && !e.lShiftDown) {
    numRenderingThreads++;
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_t && e.lShiftDown) {
    numRenderingThreads--;
    numRenderingThreads = max(numRenderingThreads, 1);
    rayTracingDone = false;
}


if(e.sdlKeyCode == SDLK_F2) {
    useAnimation = !useAnimation;
}
if(e.sdlKeyCode == SDLK_F3) {
    animationRunning = !animationRunning;
}
if(e.sdlKeyCode == SDLK_F4) {
    loopMode = cycleValue(loopMode+1, loopModes.size()-1);
}

if(e.sdlKeyCode == SDLK_F5) {
    anim.numSubFrames -= e.lControlDown ? 5 : (e.lShiftDown ? 50 : 1);
    anim.numSubFrames = max(anim.numSubFrames, 1);
}
if(e.sdlKeyCode == SDLK_F6) {
    anim.numSubFrames += e.lControlDown ? 5 : (e.lShiftDown ? 50 : 1);
}
if(e.sdlKeyCode == SDLK_F7) {
    animationStep--;
}
if(e.sdlKeyCode == SDLK_F8) {
    animationStep++;
}
if(e.sdlKeyCode == SDLK_F11 && e.lControlDown) {
    anim.removeCurrentKeyFrame();
}
if(e.sdlKeyCode == SDLK_F12 && e.lControlDown) {
    anim.reset();
}

if(e.sdlKeyCode == SDLK_z && e.lShiftDown) {
    colorAlphaFactor = max(colorAlphaFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_z && !e.lShiftDown) {
    colorAlphaFactor = min(colorAlphaFactor+adjustAmount, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_x && e.lShiftDown) {
    colorRayStrengthFactor = max(colorRayStrengthFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_x && !e.lShiftDown) {
    colorRayStrengthFactor = min(colorRayStrengthFactor+adjustAmount, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_c && e.lShiftDown) {
    attenuationAlphaFactor = max(attenuationAlphaFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_c && !e.lShiftDown) {
    attenuationAlphaFactor = min(attenuationAlphaFactor+adjustAmount, 1);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_a && e.lShiftDown) {
    redFactor = max(redFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_a && !e.lShiftDown) {
    redFactor = min(redFactor+adjustAmount, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_s && e.lShiftDown) {
    greenFactor = max(greenFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_s && !e.lShiftDown) {
    greenFactor = min(greenFactor+adjustAmount, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_d && e.lShiftDown) {
    blueFactor = max(blueFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_d && !e.lShiftDown) {
    blueFactor = min(blueFactor+adjustAmount, 1);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_f && e.lShiftDown) {
    intensityFactor = max(intensityFactor-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_f && !e.lShiftDown) {
    intensityFactor = min(intensityFactor+adjustAmount, 1);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_v && e.lShiftDown) {
    deltaOffset = max(deltaOffset-adjustAmount, 0);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_v && !e.lShiftDown) {
    deltaOffset = min(deltaOffset+adjustAmount, 1);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_p && !e.lShiftDown) {
    pixelization = min(pixelization+1, 64);
    resetCanvases();
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_p && e.lShiftDown) {
    pixelization = max(pixelization-1, 1);
    resetCanvases();
    rayTracingDone = false;
}


if(e.sdlKeyCode == SDLK_LESS) {
    anim.setFrame(anim.currentKeyFrame, anim.currentSubFrame+1);
    anim.applyToScene(*this);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_GREATER) {
    anim.setFrame(anim.currentKeyFrame, anim.currentSubFrame-1);
    anim.applyToScene(*this);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_SPACE) {
    if(e.lControlDown) {
        anim.setFrame(anim.currentKeyFrame+1, 0);
    }
    else if(e.lShiftDown) {
        anim.setFrame(anim.currentKeyFrame-1, 0);
    }
    anim.applyToScene(*this);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_PAGEUP) {
    anim.setFrame(anim.currentKeyFrame, anim.currentSubFrame + 1);
}
if(e.sdlKeyCode == SDLK_PAGEDOWN) {
    anim.setFrame(anim.currentKeyFrame, anim.currentSubFrame - 1);
}
if(e.sdlKeyCode == SDLK_HOME) {
    anim.setFrame(anim.currentKeyFrame + 1, 0);
}
if(e.sdlKeyCode == SDLK_END) {
    anim.setFrame(anim.currentKeyFrame - 1, 0);
}

if(e.sdlKeyCode == SDLK_INSERT) {
    if(useAnimation) {
        startSaving();
    }
    else {
        saveScene(false);
    }
}

if(e.sdlKeyCode == SDLK_DELETE) {
    stopSaving();
}

if(e.sdlKeyCode == SDLK_F5 && e.lShiftDown) {
    for(int i=0; i<16; i++) {
        camRotX -= PI/8.0;
        newFrame(false);
    }
}
if(e.sdlKeyCode == SDLK_F6 && e.lShiftDown) {
    for(int i=0; i<16; i++) {
        camRotY -= PI/8.0;
        newFrame(false);
    }
}

if(e.sdlKeyCode == SDLK_ASTERISK) {
    anim.applyNonCameraInfoToAllTheRest(*this);
}

if(e.sdlKeyCode == SDLK_MINUS) {
    cameraMode = 1-cameraMode;
}
if(e.sdlKeyCode == SDLK_b && !e.lShiftDown) {
    flipMode = cycleValue(flipMode+1, flipModes.size()-1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_b && e.lShiftDown) {
    flipMode = cycleValue(flipMode-1, flipModes.size()-1);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_6 && !e.lShiftDown) {
    xOffsetFactor = clamp(xOffsetFactor+adjustAmount, 0, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_6 && e.lShiftDown) {
    xOffsetFactor = clamp(xOffsetFactor-adjustAmount, 0, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_7 && !e.lShiftDown) {
    verticalFOV += 0.5;
    verticalFOV = min(180, verticalFOV);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_7 && e.lShiftDown) {
    verticalFOV -= 0.5;
    verticalFOV = max(0.5, verticalFOV);
    rayTracingDone = false;
}

if(e.sdlKeyCode == SDLK_8 && !e.lShiftDown) {
    mipFactor = clamp(mipFactor+adjustAmount, 0, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_8 && e.lShiftDown) {
    mipFactor = clamp(mipFactor-adjustAmount, 0, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_9 && !e.lShiftDown) {
    averageFactor = clamp(averageFactor+adjustAmount, 0, 1);
    rayTracingDone = false;
}
if(e.sdlKeyCode == SDLK_9 && e.lShiftDown) {
    averageFactor = clamp(averageFactor-adjustAmount, 0, 1);
    rayTracingDone = false;
}

/*if(sceneSynchronization == 2) {
syncScenes();
}*/

}
/*void keyReleased(KeyEvent e) {}
void mousePressed(MouseEvent e) {}
void mouseReleased(MouseEvent e) {}*/

void Scene3D::onMouseMotion(Events &e) {
    double dx = e.mouseX - e.pMouseX;
    double dy = e.mouseY - e.pMouseY;

    if(e.mouseDownL && e.lControlDown) {
        voxelGrid.maxX += dx;
        voxelGrid.maxY += dy;
        voxelGrid.maxX = clamp(voxelGrid.maxX, voxelGrid.minX+1, voxelGrid.w);
        voxelGrid.maxY = clamp(voxelGrid.maxY, voxelGrid.minY+1, voxelGrid.h);
        rayTracingDone = false;
    }
    if(e.mouseDownL && e.lShiftDown) {
        voxelGrid.minX += dx;
        voxelGrid.minY += dy;
        voxelGrid.minX = clamp(voxelGrid.minX, 0, voxelGrid.maxX-1);
        voxelGrid.minY = clamp(voxelGrid.minY, 0, voxelGrid.maxY-1);
        rayTracingDone = false;
    }
    if(e.mouseDownM && e.lShiftDown) {
        voxelGrid.maxZ -= dy;
        voxelGrid.maxZ = clamp(voxelGrid.maxZ, voxelGrid.minZ+1, voxelGrid.d);
        rayTracingDone = false;
    }
    if(e.mouseDownM && e.lControlDown) {
        voxelGrid.minZ -= dy;
        voxelGrid.minZ = clamp(voxelGrid.minZ, 0, voxelGrid.maxZ-1);
        rayTracingDone = false;
    }
    if(e.mouseDownL && !e.lControlDown && !e.lShiftDown) {
        updateCamera(dx, dy, 0, 0);
    }
    if(e.mouseDownR && !e.lControlDown && !e.lShiftDown) {
        updateCamera(dx, 0, dy, 1);
    }
    if(e.mouseDownM && !e.lControlDown && !e.lShiftDown) {
        updateCamera(dx, 0, dy, 2);
    }

    /*if(sceneSynchronization == 2) {
    syncScenes();
}*/
}
