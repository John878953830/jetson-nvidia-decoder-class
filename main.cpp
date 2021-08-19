#include <iostream>
#include "decoder_h264.h"
#include <thread>

int main()
{
    /**
     * @brief new a class
     */
    decoder_h264 *tmp = new decoder_h264(1440, 1920);
    /**
     * @brief read data from the stream h264 which saved from M300 main camera
     */
    char inpath[] = "m1.h264";
    auto indata = new ifstream(inpath);

    int counter = 0;
    while (counter < 50)
    {
        char *data = (char *)malloc(400000);
        indata->read(data, 400000);
        /**
         * @brief read stream data into queue for decoding, this function must be called periodly
         */
        tmp->stream_in_data((uint8_t *)data, 400000);

        counter++;
    }
    /**
     * @brief start the decode proc, this proc is blocking , so you would better do this in another thread
     */
    tmp->decode_proc();
    return 0;
}
