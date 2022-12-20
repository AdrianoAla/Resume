#include "helpers.h"
#include <stdio.h>
#include <math.h>

int red = 0;
int blue = 0;
int green = 0;
int count = 0;

int gy[3][3] =
{
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
};

int gx[3][3] =
{
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

int red_array[3][3];
int blue_array[3][3];
int green_array[3][3];


// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE pixel = image[i][j];
            int average = round((pixel.rgbtBlue + pixel.rgbtGreen + pixel.rgbtRed) / 3.0);
            image[i][j].rgbtRed = average;
            image[i][j].rgbtBlue = average;
            image[i][j].rgbtGreen = average;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{

    RGBTRIPLE image_copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][width - j - 1] = image_copy[i][j];
        }
    }

    return;
}

void add_pixel_data(RGBTRIPLE pixel)
{
    red += pixel.rgbtRed;
    green += pixel.rgbtGreen;
    blue += pixel.rgbtBlue;
    count ++;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{

    RGBTRIPLE image_copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Initialize all variables

            red = 0;
            green = 0;
            blue = 0;
            count = 0;

            // Here we will add pixel data to everything
            // Center
            add_pixel_data(image_copy[i][j]);

            if (i > 0)
            {
                // Top
                add_pixel_data(image_copy[i - 1][j]);

                if (j > 0)
                {
                    // Top left
                    add_pixel_data(image_copy[i - 1][j - 1]);
                }

                if (j < width - 1)
                {
                    // Top right
                    add_pixel_data(image_copy[i - 1][j + 1]);
                }
            }

            if (i < height - 1)
            {
                // Bottom
                add_pixel_data(image_copy[i + 1][j]);

                if (j > 0)
                {
                    // Bottom left
                    add_pixel_data(image_copy[i + 1][j - 1]);

                }
                if (j < width - 1)
                {
                    // Bottom right
                    add_pixel_data(image_copy[i + 1][j + 1]);

                }
            }

            if (j > 0)
            {
                // Left
                add_pixel_data(image_copy[i][j - 1]);
            }

            if (j < width - 1)
            {
                // Right
                add_pixel_data(image_copy[i][j + 1]);
            }

            // Average red, green and blue
            red = round((float) red / count);
            blue = round((float) blue / count);
            green = round((float) green / count);

            image[i][j].rgbtRed = red;
            image[i][j].rgbtBlue = blue;
            image[i][j].rgbtGreen = green;

        }
    }
}

void process_pixel_edges(int x, int y, RGBTRIPLE pixel)
{
    red_array[y][x] = pixel.rgbtRed;
    blue_array[y][x] = pixel.rgbtBlue;
    green_array[y][x] = pixel.rgbtGreen;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{

    RGBTRIPLE image_copy[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    red_array[k][l] = 0;
                    blue_array[k][l] = 0;
                    green_array[k][l] = 0;
                }
            }

            process_pixel_edges(1, 1, image_copy[i][j]);

            if (i > 0)
            {
                // Top
                process_pixel_edges(0, 1, image_copy[i - 1][j]);

                if (j > 0)
                {
                    // Top lefts
                    process_pixel_edges(0, 0, image_copy[i - 1][j - 1]);
                }

                if (j < width - 1)
                {
                    // Top right
                    process_pixel_edges(0, 2, image_copy[i - 1][j + 1]);
                }
            }

            if (i < height - 1)
            {
                // Bottom
                process_pixel_edges(2, 1, image_copy[i + 1][j]);

                if (j > 0)
                {
                    // Bottom left
                    process_pixel_edges(2, 0, image_copy[i + 1][j - 1]);

                }
                if (j < width - 1)
                {
                    // Bottom right
                    process_pixel_edges(2, 2, image_copy[i + 1][j + 1]);

                }
            }

            if (j > 0)
            {
                // Left
                process_pixel_edges(1, 0, image_copy[i][j - 1]);

            }

            if (j < width - 1)
            {
                // Right
                process_pixel_edges(1, 2, image_copy[i][j + 1]);
            }

            // Creating variables for storing the values of the pixles after they are multiplied with gx and gy

            int red_gx[3][3];
            int red_gy[3][3];

            int green_gx[3][3];
            int green_gy[3][3];

            int blue_gx[3][3];
            int blue_gy[3][3];

            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    red_gx[k][l] = red_array[k][l] * gx[k][l];
                    red_gy[k][l] = red_array[k][l] * gy[k][l];

                    green_gx[k][l] = green_array[k][l] * gx[k][l];
                    green_gy[k][l] = green_array[k][l] * gy[k][l];

                    blue_gx[k][l] = blue_array[k][l] * gx[k][l];
                    blue_gy[k][l] = blue_array[k][l] * gy[k][l];
                }
            }

            // Getting the sum of these values

            int red_sum_gx = 0;
            int red_sum_gy = 0;

            int blue_sum_gx = 0;
            int blue_sum_gy = 0;

            int green_sum_gx = 0;
            int green_sum_gy = 0;


            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    red_sum_gx += red_gx[k][l];
                    red_sum_gy += red_gy[k][l];

                    blue_sum_gx += blue_gx[k][l];
                    blue_sum_gy += blue_gy[k][l];

                    green_sum_gx += green_gx[k][l];
                    green_sum_gy += green_gy[k][l];
                }
            }

            int final_red = round(sqrt(pow(red_sum_gx, 2) + pow(red_sum_gy, 2)));
            if (final_red > 255)
            {
                final_red = 255;
            }

            int final_green = round(sqrt(pow(green_sum_gx, 2) + pow(green_sum_gy, 2)));
            if (final_green > 255)
            {
                final_green = 255;
            }

            int final_blue = round(sqrt(pow(blue_sum_gx, 2) + pow(blue_sum_gy, 2)));
            if (final_blue > 255)
            {
                final_blue = 255;
            }

            int final_value = round((final_red + final_green + final_blue) / 3.0);

            image[i][j].rgbtRed = final_red;
            image[i][j].rgbtBlue = final_blue;
            image[i][j].rgbtGreen = final_green;
        }
    }
}
