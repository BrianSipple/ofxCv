#include "Helpers.h"
#include "Utilities.h"

namespace ofxCv {

	using namespace cv;
	
	void loadImage(Mat& mat, string filename) {
		mat = imread(ofToDataPath(filename));
	}
	
	void saveImage(Mat mat, string filename) {
		imwrite(ofToDataPath(filename), mat);
	}
	
	ofMatrix4x4 makeMatrix(Mat rotation, Mat translation) {
		Mat rot3x3;
		if(rotation.rows == 3 && rotation.cols == 3) {
			rot3x3 = rotation;
		} else {
			Rodrigues(rotation, rot3x3);
		}
		double* rm = rot3x3.ptr<double>(0);
		double* tm = translation.ptr<double>(0);
		return ofMatrix4x4(rm[0], rm[3], rm[6], 0.0f,
											 rm[1], rm[4], rm[7], 0.0f,
											 rm[2], rm[5], rm[8], 0.0f,
											 tm[0], tm[1], tm[2], 1.0f);
	}
	
	void drawMat(Mat& mat, float x, float y) {
		drawMat(mat, x, y, mat.cols, mat.rows);
	}
	
	void drawMat(Mat& mat, float x, float y, float width, float height) {
		int glType;
		Mat buffer;
		if(mat.depth() != CV_8U) {
			mat.convertTo(buffer, CV_8U);
		} else {
			buffer = mat;
		}
		if(mat.channels() == 1) {
			glType = GL_LUMINANCE;
		} else {
			glType = GL_RGB;
		}
		ofTexture tex;
		int w = buffer.cols;
		int h = buffer.rows;
		tex.allocate(w, h, glType);
		tex.loadData(buffer.ptr(), w, h, glType);
		tex.draw(x, y, width, height);
	}
	
	void applyMatrix(const ofMatrix4x4& matrix) {
		glMultMatrixf((GLfloat*) matrix.getPtr());
	}
	
	ofVec2f findMaxLocation(Mat& mat) {
		double minVal, maxVal;
		cv::Point minLoc, maxLoc;
		minMaxLoc(mat, &minVal, &maxVal, &minLoc, &maxLoc);
		return ofVec2f(maxLoc.x, maxLoc.y);
	}
	
	void getBoundingBox(ofImage& img, ofRectangle& box, int thresh, bool invert) {
		Mat mat = toCv(img);
		int flags = (invert ? THRESH_BINARY_INV : THRESH_BINARY);
		
		Mat rowMat = meanRows(mat);
		threshold(rowMat, rowMat, thresh, 255, flags);
		box.y = findFirst(rowMat, 255);
		box.height = findLast(rowMat, 255);
		box.height -= box.y;
		
		Mat colMat = meanCols(mat);
		threshold(colMat, colMat, thresh, 255, flags);
		box.x = findFirst(colMat, 255);
		box.width = findLast(colMat, 255);
		box.width -= box.x;
	}
	
	int forceOdd(int x) {
		return (x / 2) * 2 + 1;
	}
	
	void medianBlur(ofImage& img, int size) {
		size = forceOdd(size);
		Mat mat = toCv(img);
		medianBlur(mat, mat, size);
	}
	
	void warpPerspective(ofImage& src, ofImage& dst, Mat& m, int flags) {
		Mat srcMat = toCv(src);
		Mat dstMat = toCv(dst);
		warpPerspective(srcMat, dstMat, m, dstMat.size(), flags);
	}
	
	void warpPerspective(ofPixels& src, ofPixels& dst, Mat& m, int flags) {
		Mat srcMat = toCv(src);
		Mat dstMat = toCv(dst);
		warpPerspective(srcMat, dstMat, m, dstMat.size(), flags);
	}
	
	void resize(ofImage& source, ofImage& destination, int interpolation) {
		Mat sourceMat = toCv(source);
		Mat destinationMat = toCv(destination);
		resize(sourceMat, destinationMat, destinationMat.size(), 0, 0, interpolation);
	}
	
	void resize(ofImage& source, ofImage& destination, float xScale, float yScale, int interpolation) {
		ofImageType sourceType = source.getPixelsRef().getImageType();
		destination.allocate(source.getWidth() * xScale, source.getHeight() * yScale, sourceType);
		resize(source, destination, interpolation);
	}
	
	int findFirst(const Mat& arr, unsigned char target) {
		for(int i = 0; i < arr.rows; i++) {
			if(arr.at<unsigned char>(i) == target) {
				return i;
			}
		}
		return 0;
	}
	
	int findLast(const Mat& arr, unsigned char target) {
		for(int i = arr.rows - 1; i >= 0; i--) {
			if(arr.at<unsigned char>(i) == target) {
				return i;
			}
		}
		return 0;
	}
	
	Mat meanCols(const Mat& mat) {
		Mat colMat(mat.cols, 1, mat.type());
		for(int i = 0; i < mat.cols; i++) {
			colMat.row(i) = mean(mat.col(i));
		}	
		return colMat;
	}
	
	Mat meanRows(const Mat& mat) {
		Mat rowMat(mat.rows, 1, mat.type());
		for(int i = 0; i < mat.rows; i++) {
			rowMat.row(i) = mean(mat.row(i));
		}
		return rowMat;
	}
	
	Mat sumCols(const Mat& mat) {
		Mat colMat(mat.cols, 1, CV_32FC1);
		for(int i = 0; i < mat.cols; i++) {
			colMat.row(i) = sum(mat.col(i));
		}	
		return colMat;
	}
	
	Mat sumRows(const Mat& mat) {
		Mat rowMat(mat.rows, 1, CV_32FC1);
		for(int i = 0; i < mat.rows; i++) {
			rowMat.row(i) = sum(mat.row(i));
		}
		return rowMat;
	}
	
	Mat minCols(const Mat& mat) {
		Mat colMat(mat.cols, 1, CV_32FC1);
		double minVal, maxVal;
		for(int i = 0; i < mat.cols; i++) {
			minMaxLoc(mat.col(i), &minVal, &maxVal); 
			colMat.row(i) = minVal;
		}	
		return colMat;
	}
	
	Mat minRows(const Mat& mat) {
		Mat rowMat(mat.rows, 1, CV_32FC1);
		double minVal, maxVal;
		for(int i = 0; i < mat.rows; i++) {
			minMaxLoc(mat.row(i), &minVal, &maxVal); 
			rowMat.row(i) = minVal;
		}
		return rowMat;
	}
	
	Mat maxCols(const Mat& mat) {
		Mat colMat(mat.cols, 1, CV_32FC1);
		double minVal, maxVal;
		for(int i = 0; i < mat.cols; i++) {
			minMaxLoc(mat.col(i), &minVal, &maxVal); 
			colMat.row(i) = maxVal;
		}	
		return colMat;
	}
	
	Mat maxRows(const Mat& mat) {
		Mat rowMat(mat.rows, 1, CV_32FC1);
		double minVal, maxVal;
		for(int i = 0; i < mat.rows; i++) {
			minMaxLoc(mat.row(i), &minVal, &maxVal); 
			rowMat.row(i) = maxVal;
		}
		return rowMat;
	}
	
	void drawHighlightString(string text, ofPoint position, ofColor background, ofColor foreground) {
		drawHighlightString(text, position.x, position.y, background, foreground);
	}
	
	void drawHighlightString(string text, int x, int y, ofColor background, ofColor foreground) {
		ofPushStyle();
		int textWidth =  10 + text.length() * 8;
		ofSetColor(background);
		ofFill();
		ofRect(x - 5, y - 12, textWidth, 20);
		ofSetColor(foreground);
		ofDrawBitmapString(text, x, y);
		ofPopStyle();
	}

}