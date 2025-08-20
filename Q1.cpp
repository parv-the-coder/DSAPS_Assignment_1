#include<iostream>
#include<cmath>
#include<opencv2/opencv.hpp>
using namespace std;


int *** convimg(cv::Mat &img,int &height,int &width)
{
    int ***image = new int **[height];
    for (int i = 0; i < height; i++)
    {
        image[i] = new int *[width];

        for (int j = 0; j < width; j++)
        {
            image[i][j] = new int[3]; // RGB
        }
    }

    // filling the pixel value
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cv::Vec3b pixel = img.at<cv::Vec3b>(i, j);
            image[i][j][0] = pixel[0]; // B
            image[i][j][1] = pixel[1]; // G
            image[i][j][2] = pixel[2]; // R
        }
    }

    return image;
}



// minimum vertical seam
int *searchverti(double **energy, int height, int width)
{
    double **dp = new double *[height]; //  min energy to reach (i,j)
    int **track = new int *[height];    // keeping track of previous column in seam
    for (int i = 0; i < height; i++)
    {
        dp[i] = new double[width];
        track[i] = new int[width];
    }

    for (int j = 0; j < width; j++)
    {
        dp[0][j] = energy[0][j];
    }

    for (int i = 1; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            double minimum = dp[i - 1][j]; // above
            int idx = j;

            if ((j > 0) && (dp[i - 1][j - 1] < minimum)) // top left
            {
                minimum = dp[i - 1][j - 1];
                idx = j - 1;
            }
            if ((j < width - 1) && (dp[i - 1][j + 1] < minimum)) // top right
            {
                minimum = dp[i - 1][j + 1];
                idx = j + 1;
            }

            dp[i][j] = energy[i][j] + minimum; // total energy
            track[i][j] = idx;                 // storing old col value
        }
    }

    // backtack
    int *seam = new int[height];
    int minidx = 0;
    double minimum = dp[height - 1][0];

    for (int j = 1; j < width; j++)
        if (dp[height - 1][j] < minimum) // start bottom row
        {
            minimum = dp[height - 1][j]; // col with min energy
            minidx = j;
        }
    seam[height - 1] = minidx;

    // tracking upward
    for (int i = height - 2; i >= 0; i--)
    {
        seam[i] = track[i + 1][seam[i + 1]];
    }

    return seam;
}



// calculating energy
double **calenergy(int ***image, int height, int width)
{
    double **energy = new double *[height];
    for (int i = 0; i < height; i++)
    {
        energy[i] = new double[width];
    }

    // going through each pixel
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // adjacent pixels for  calculation of gradient
            int left, right, up, down;

            if (j == 0)
            {
                left = 0;
            }
            else
            {
                left = image[i][j - 1][0];
            }

            if (j == width - 1)
            {
                right = 0;
            }
            else
            {
                right = image[i][j + 1][0];
            }

            if (i == 0)
            {
                up = 0;
            }
            else
            {
                up = image[i - 1][j][0];
            }

            if (i == height - 1)
            {
                down = 0;
            }
            else
            {
                down = image[i + 1][j][0];
            }

            // calculating gradient
            int dx = right - left; // x-axis gradient
            int dy = down - up;    // y-axis gradient

            energy[i][j] = sqrt(dx * dx + dy * dy);
        }
    }
    return energy;
}



// cutting down vertical
void cutverti(int ***&image, int *seam, int &height, int &width)
{

    for (int i = 0; i < height; i++)
        for (int j = seam[i]; j < width - 1; j++)
        {
            image[i][j][0] = image[i][j + 1][0];
            image[i][j][1] = image[i][j + 1][1];
            image[i][j][2] = image[i][j + 1][2];
        }
    width--;
}



// transposing now cutting horizontal
int ***cuthori(int ***image, int height, int width)
{
    int ***transimg = new int **[width];
    for (int i = 0; i < width; i++)
    {
        transimg[i] = new int *[height];
        for (int j = 0; j < height; j++)
        {
            transimg[i][j] = new int[3];
            transimg[i][j][0] = image[j][i][0];
            transimg[i][j][1] = image[j][i][1];
            transimg[i][j][2] = image[j][i][2];
        }
    }
    return transimg;
}



// conv back arr to mat
cv::Mat convMat(int ***image, int height, int width)
{
    cv::Mat mat(height, width, CV_8UC3);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            mat.at<cv::Vec3b>(i, j) = cv::Vec3b(image[i][j][0], image[i][j][1], image[i][j][2]);
        }
    }
    return mat;
}



int main()
{
    string inppath;
    cout<<"Enter Path of image: ";
    cin>>inppath;               // taking path from user

    cv::Mat img = cv::imread(inppath);      // reading image

    //checking imaged loaded properly
    if(img.empty())
    {
        cout<<"Error in loading image"<<endl;
        return -1;
    }

    int height = img.rows;
    int width = img.cols;
    cout << "Original Dimensions: " << height << " x " << width << endl;

    int nheight,nwidth;
    cout<<"Enter Height: ";
    cin>>nheight;
    cout<<"Enter Width: ";
    cin>>nwidth;

    // can't be 0 or negative
    if (nheight <= 0 || nwidth <= 0)
    {
        cout << "Error: Target dimensions must be positive" << endl;
        return -1;
    }

    // can't  be greater than original dimensions
    if (nheight > img.rows || nwidth > img.cols)
    {
        cout << "Error: Target dimensions cannot be larger than original" << endl;
        return -1;
    }

    // converting image to 3D
    int ***image = convimg(img, height, width);

    cout<<"Target Dimensions: "<<nheight<<" x "<< nwidth<<endl;

    // reducing width
    while (width > nwidth)
    {
        double **energy = calenergy(image, height, width);
        int *seam = searchverti(energy, height, width);
        cutverti(image, seam, height, width);
    }

    // rotating
    image = cuthori(image, height, width);

    // swapping
    int temp = height;
    height = width;
    width = temp;

    while (width > nheight)
    {
        double **energy = calenergy(image, height, width);
        int *seam = searchverti(energy, height, width);
        cutverti(image, seam, height, width);
    }

    // back to original
    image = cuthori(image, height, width);
    // swapping
    temp = height;
    height = width;
    width = temp;

    // converting back to image
    cv::Mat output = convMat(image, height, width);

    string outputName;
    cout << "Enter the name for output image (including extension, for e.g. output.jpeg): ";
    cin >> outputName;

    // save the image
    cv::imwrite(outputName, output);
    cout << "Resized image saved as " << outputName << endl;

    return 0;
}