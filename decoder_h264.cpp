//
// Created by daisy on 2021/8/13.
//

#include "decoder_h264.h"
decoder_h264::decoder_h264() = default;
decoder_h264::~decoder_h264() = default;
decoder_h264::decoder_h264(int h, int w)
{
    height = h;
    width = w;
    cv::Mat m(height, width, CV_8UC3);
    mat_cache = m.clone();
    buf1_use_flag = -1;
    clone_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_create(&handle_image_save_thread, NULL, image_save_thread, (void *)this);
    pthread_create(&handle_image_cvt, NULL, image_cvt, (void *)this);
}
void decoder_h264::set_defaults(context_t *ctx)
{
    memset(ctx, 0, sizeof(context_t));

    ctx->decoder_pixfmt = V4L2_PIX_FMT_H264;
    ctx->fullscreen = false;
    ctx->window_height = 0;
    ctx->window_width = 0;
    ctx->window_x = 0;
    ctx->window_y = 0;
    ctx->out_pixfmt = 1;
    ctx->fps = 30;
    ctx->output_plane_mem_type = V4L2_MEMORY_MMAP;
    ctx->capture_plane_mem_type = V4L2_MEMORY_DMABUF;
    ctx->vp9_file_header_flag = 0;
    ctx->vp8_file_header_flag = 0;
    ctx->stress_test = 1;
    ctx->copy_timestamp = false;
    ctx->flag_copyts = false;
    ctx->start_ts = 0;
    ctx->file_count = 1;
    ctx->dec_fps = 30;
    ctx->dst_dma_fd = -1;
    ctx->bLoop = false;
    ctx->bQueue = false;
    ctx->loop_count = 0;
    ctx->max_perf = 0;
    ctx->extra_cap_plane_buffer = 1;
    ctx->blocking_mode = 1;
    pthread_mutex_init(&ctx->queue_lock, NULL);
    pthread_cond_init(&ctx->queue_cond, NULL);
    return;
}

void decoder_h264::abort()
{
    ctx.got_error = true;
    ctx.dec->abort();
}

int decoder_h264::read_decoder_input_chunk(ifstream *stream, NvBuffer *buffer)
{
    /* Length is the size of the buffer in bytes */
    //streamsize bytes_to_read = MIN(CHUNK_SIZE, buffer->planes[0].length);
    //printf("input read in chunk function\n");
    if (frame_counter == 0)
    {
        frame_counter++;
        memcpy((char *)buffer->planes[0].data, iframe_h20_shot_photo_h264, sizeof(iframe_h20_shot_photo_h264));
        buffer->planes[0].bytesused = sizeof(iframe_h20_shot_photo_h264);
    }
    else
    {
        frame_counter++;
        if (!cq_queue.empty())
        {
            CQ tmp = cq_queue.front();
            memcpy((char *)buffer->planes[0].data, tmp.data, tmp.len);
            buffer->planes[0].bytesused = tmp.len;
            if (cq_queue.size() > 1)
            {
                cach_queue.push(tmp);
                cq_queue.pop();
            }
        }
        if (cach_queue.size() > 20)
        {
            CQ tmp2 = cach_queue.front();
            free(tmp2.data);
            cach_queue.pop();
        }
    }
    ///stream->read((char *)buffer->planes[0].data, bytes_to_read);
    /* NOTE: It is necessary to set bytesused properly, so that decoder knows how
             many bytes in the buffer are valid. */
    //buffer->planes[0].bytesused = stream->gcount();
    printf("read size is %d\n", buffer->planes[0].bytesused);
    printf("out read decoder input %d\n", frame_counter);
    return 0;
}

void decoder_h264::query_and_set_capture()
{
    NvVideoDecoder *dec = ctx.dec;
    struct v4l2_format format;
    struct v4l2_crop crop;
    int32_t min_dec_capture_buffers;
    int ret = 0;
    int error = 0;
    uint32_t window_width;
    uint32_t window_height;
    NvBufferCreateParams input_params = {0};
    NvBufferCreateParams cParams = {0};

    /* Get capture plane format from the decoder.
       This may change after resolution change event.
       Refer ioctl VIDIOC_G_FMT */
    ret = dec->capture_plane.getFormat(format);
    TEST_ERROR(ret < 0,
               "Error: Could not get format from decoder capture plane", error);

    /* Get the display resolution from the decoder.
       Refer ioctl VIDIOC_G_CROP */
    ret = dec->capture_plane.getCrop(crop);
    TEST_ERROR(ret < 0,
               "Error: Could not get crop from decoder capture plane", error);

    cout << "Video Resolution: " << crop.c.width << "x" << crop.c.height
         << endl;
    ctx.display_height = crop.c.height;
    ctx.display_width = crop.c.width;
#ifdef USE_NVBUF_TRANSFORM_API
    if (ctx.dst_dma_fd != -1)
    {
        NvBufferDestroy(ctx.dst_dma_fd);
        ctx.dst_dma_fd = -1;
    }
    /* Create PitchLinear output buffer for transform. */
    input_params.payloadType = NvBufferPayload_SurfArray;
    input_params.width = crop.c.width;
    input_params.height = crop.c.height;
    input_params.layout = NvBufferLayout_Pitch;
    if (ctx.out_pixfmt == 1)
        input_params.colorFormat = NvBufferColorFormat_NV12;
    else if (ctx.out_pixfmt == 2)
        input_params.colorFormat = NvBufferColorFormat_YUV420;
    else if (ctx.out_pixfmt == 3)
        input_params.colorFormat = NvBufferColorFormat_NV16;
    else if (ctx.out_pixfmt == 4)
        input_params.colorFormat = NvBufferColorFormat_NV24;

    input_params.nvbuf_tag = NvBufferTag_VIDEO_CONVERT;

    ret = NvBufferCreateEx(&(ctx.dst_dma_fd), &input_params);
    TEST_ERROR(ret == -1, "create dmabuf failed", error);
#else
    /* For file write, first deinitialize output and capture planes
       of video converter and then use the new resolution from
       decoder event resolution change. */
    if (ctx.conv)
    {
        ret = sendEOStoConverter(ctx);
        TEST_ERROR(ret < 0,
                   "Error while queueing EOS buffer on converter output",
                   error);

        ctx.conv->capture_plane.waitForDQThread(2000);

        ctx.conv->output_plane.deinitPlane();
        ctx.conv->capture_plane.deinitPlane();

        while (!ctx.conv_output_plane_buf_queue->empty())
        {
            ctx.conv_output_plane_buf_queue->pop();
        }
    }
#endif

    if (!ctx.disable_rendering)
    {
        /* Destroy the old instance of renderer as resolution might have changed. */
        delete ctx.renderer;

        if (ctx.fullscreen)
        {
            /* Required for fullscreen. */
            window_width = window_height = 0;
        }
        else if (ctx.window_width && ctx.window_height)
        {
            /* As specified by user on commandline. */
            window_width = ctx.window_width;
            window_height = ctx.window_height;
        }
        else
        {
            /* Resolution got from the decoder. */
            window_width = crop.c.width;
            window_height = crop.c.height;
        }

        /* If height or width are set to zero, EglRenderer creates a fullscreen
           window for rendering. */
        ctx.renderer =
            NvEglRenderer::createEglRenderer("renderer0", window_width,
                                             window_height, ctx.window_x,
                                             ctx.window_y);
        TEST_ERROR(!ctx.renderer,
                   "Error in setting up renderer. "
                   "Check if X is running or run with --disable-rendering",
                   error);
        if (ctx.stats)
        {
            /* Enable profiling for renderer if stats are requested. */
            ctx.renderer->enableProfiling();
        }

        /* Set fps for rendering. */
        ctx.renderer->setFPS(ctx.fps);
    }

    /* deinitPlane unmaps the buffers and calls REQBUFS with count 0 */
    dec->capture_plane.deinitPlane();
    if (ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
    {
        for (int index = 0; index < ctx.numCapBuffers; index++)
        {
            if (ctx.dmabuff_fd[index] != 0)
            {
                ret = NvBufferDestroy(ctx.dmabuff_fd[index]);
                TEST_ERROR(ret < 0, "Failed to Destroy NvBuffer", error);
            }
        }
    }

    /* Not necessary to call VIDIOC_S_FMT on decoder capture plane.
       But decoder setCapturePlaneFormat function updates the class variables */
    ret = dec->setCapturePlaneFormat(format.fmt.pix_mp.pixelformat,
                                     format.fmt.pix_mp.width,
                                     format.fmt.pix_mp.height);
    TEST_ERROR(ret < 0, "Error in setting decoder capture plane format", error);

    ctx.video_height = format.fmt.pix_mp.height;
    ctx.video_width = format.fmt.pix_mp.width;
    /* Get the minimum buffers which have to be requested on the capture plane. */
    ret = dec->getMinimumCapturePlaneBuffers(min_dec_capture_buffers);
    TEST_ERROR(ret < 0,
               "Error while getting value of minimum capture plane buffers",
               error);

    /* Request (min + extra) buffers, export and map buffers. */
    if (ctx.capture_plane_mem_type == V4L2_MEMORY_MMAP)
    {
        /* Request, Query and export decoder capture plane buffers.
           Refer ioctl VIDIOC_REQBUFS, VIDIOC_QUERYBUF and VIDIOC_EXPBUF */
        ret =
            dec->capture_plane.setupPlane(V4L2_MEMORY_MMAP,
                                          min_dec_capture_buffers + ctx.extra_cap_plane_buffer, false,
                                          false);
        TEST_ERROR(ret < 0, "Error in decoder capture plane setup", error);
    }
    else if (ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
    {
        /* Set colorformats for relevant colorspaces. */
        switch (format.fmt.pix_mp.colorspace)
        {
        case V4L2_COLORSPACE_SMPTE170M:
            if (format.fmt.pix_mp.quantization == V4L2_QUANTIZATION_DEFAULT)
            {
                cout << "Decoder colorspace ITU-R BT.601 with standard range luma (16-235)" << endl;
                cParams.colorFormat = NvBufferColorFormat_NV12;
            }
            else
            {
                cout << "Decoder colorspace ITU-R BT.601 with extended range luma (0-255)" << endl;
                cParams.colorFormat = NvBufferColorFormat_NV12_ER;
            }
            break;
        case V4L2_COLORSPACE_REC709:
            if (format.fmt.pix_mp.quantization == V4L2_QUANTIZATION_DEFAULT)
            {
                cout << "Decoder colorspace ITU-R BT.709 with standard range luma (16-235)" << endl;
                cParams.colorFormat = NvBufferColorFormat_NV12_709;
            }
            else
            {
                cout << "Decoder colorspace ITU-R BT.709 with extended range luma (0-255)" << endl;
                cParams.colorFormat = NvBufferColorFormat_NV12_709_ER;
            }
            break;
        case V4L2_COLORSPACE_BT2020:
        {
            cout << "Decoder colorspace ITU-R BT.2020" << endl;
            cParams.colorFormat = NvBufferColorFormat_NV12_2020;
        }
        break;
        default:
            cout << "supported colorspace details not available, use default" << endl;
            if (format.fmt.pix_mp.quantization == V4L2_QUANTIZATION_DEFAULT)
            {
                cout << "Decoder colorspace ITU-R BT.601 with standard range luma (16-235)" << endl;
                cParams.colorFormat = NvBufferColorFormat_NV12;
            }
            else
            {
                cout << "Decoder colorspace ITU-R BT.601 with extended range luma (0-255)" << endl;
                cParams.colorFormat = NvBufferColorFormat_NV12_ER;
            }
            break;
        }

        ctx.numCapBuffers = min_dec_capture_buffers + ctx.extra_cap_plane_buffer;

        /* Create decoder capture plane buffers. */
        for (int index = 0; index < ctx.numCapBuffers; index++)
        {
            cParams.width = crop.c.width;
            cParams.height = crop.c.height;
            cParams.layout = NvBufferLayout_BlockLinear;
            cParams.payloadType = NvBufferPayload_SurfArray;
            cParams.nvbuf_tag = NvBufferTag_VIDEO_DEC;
            ret = NvBufferCreateEx(&(ctx.dmabuff_fd[index]), &cParams);
            TEST_ERROR(ret < 0, "Failed to create buffers", error);
        }

        /* Request buffers on decoder capture plane.
           Refer ioctl VIDIOC_REQBUFS */
        ret = dec->capture_plane.reqbufs(V4L2_MEMORY_DMABUF, ctx.numCapBuffers);
        TEST_ERROR(ret, "Error in request buffers on capture plane", error);
    }

#ifndef USE_NVBUF_TRANSFORM_API
    if (ctx.conv)
    {
        /* Set Converter output plane format.
           Refer ioctl VIDIOC_S_FMT */
        ret = ctx.conv->setOutputPlaneFormat(format.fmt.pix_mp.pixelformat,
                                             format.fmt.pix_mp.width,
                                             format.fmt.pix_mp.height,
                                             V4L2_NV_BUFFER_LAYOUT_BLOCKLINEAR);
        TEST_ERROR(ret < 0, "Error in converter output plane set format",
                   error);

        /* Set Converter capture plane format.
           Refer ioctl VIDIOC_S_FMT */
        ret = ctx.conv->setCapturePlaneFormat((ctx.out_pixfmt == 1 ? V4L2_PIX_FMT_NV12M : V4L2_PIX_FMT_YUV420M),
                                              crop.c.width,
                                              crop.c.height,
                                              V4L2_NV_BUFFER_LAYOUT_PITCH);
        TEST_ERROR(ret < 0, "Error in converter capture plane set format",
                   error);

        /* Set Converter crop rectangle. */
        ret = ctx.conv->setCropRect(0, 0, crop.c.width, crop.c.height);
        TEST_ERROR(ret < 0, "Error while setting crop rect", error);

        if (ctx.rescale_method)
        {
            /* Rescale full range [0-255] to limited range [16-235].
               Refer V4L2_CID_VIDEO_CONVERT_YUV_RESCALE_METHOD */
            ret = ctx.conv->setYUVRescale(ctx->rescale_method);
            TEST_ERROR(ret < 0, "Error while setting YUV rescale", error);
        }

        /* Request buffers on converter output plane.
           Refer ioctl VIDIOC_REQBUFS */
        ret =
            ctx.conv->output_plane.setupPlane(V4L2_MEMORY_DMABUF,
                                              dec->capture_plane.getNumBuffers(), false, false);
        TEST_ERROR(ret < 0, "Error in converter output plane setup", error);

        /* Request, Query and export converter capture plane buffers.
           Refer ioctl VIDIOC_REQBUFS, VIDIOC_QUERYBUF and VIDIOC_EXPBUF */
        ret =
            ctx.conv->capture_plane.setupPlane(V4L2_MEMORY_MMAP,
                                               dec->capture_plane.getNumBuffers(), true, false);
        TEST_ERROR(ret < 0, "Error in converter capture plane setup", error);

        /* Converter output plane STREAMON.
           Refer ioctl VIDIOC_STREAMON */
        ret = ctx.conv->output_plane.setStreamStatus(true);
        TEST_ERROR(ret < 0, "Error in converter output plane streamon", error);

        /* Converter capture plane STREAMON.
           Refer ioctl VIDIOC_STREAMON */
        ret = ctx.conv->capture_plane.setStreamStatus(true);
        TEST_ERROR(ret < 0, "Error in converter output plane streamoff", error);

        /* Add all empty conv output plane buffers to conv_output_plane_buf_queue. */
        for (uint32_t i = 0; i < ctx.conv->output_plane.getNumBuffers(); i++)
        {
            ctx->conv_output_plane_buf_queue->push(ctx.conv->output_plane.getNthBuffer(i));
        }

        /* Enqueue converter capture plane buffers. */
        for (uint32_t i = 0; i < ctx.conv->capture_plane.getNumBuffers(); i++)
        {
            struct v4l2_buffer v4l2_buf;
            struct v4l2_plane planes[MAX_PLANES];

            memset(&v4l2_buf, 0, sizeof(v4l2_buf));
            memset(planes, 0, sizeof(planes));

            v4l2_buf.index = i;
            v4l2_buf.m.planes = planes;
            ret = ctx.conv->capture_plane.qBuffer(v4l2_buf, NULL);
            TEST_ERROR(ret < 0, "Error Qing buffer at converter capture plane",
                       error);
        }
        /* Start deque thread for converter output plane. */
        ctx.conv->output_plane.startDQThread(ctx);
        /* Start deque thread for converter capture plane. */
        ctx.conv->capture_plane.startDQThread(ctx);
    }
#endif

    /* Decoder capture plane STREAMON.
       Refer ioctl VIDIOC_STREAMON */
    ret = dec->capture_plane.setStreamStatus(true);
    TEST_ERROR(ret < 0, "Error in decoder capture plane streamon", error);

    /* Enqueue all the empty decoder capture plane buffers. */
    for (uint32_t i = 0; i < dec->capture_plane.getNumBuffers(); i++)
    {
        struct v4l2_buffer v4l2_buf;
        struct v4l2_plane planes[MAX_PLANES];

        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        memset(planes, 0, sizeof(planes));

        v4l2_buf.index = i;
        v4l2_buf.m.planes = planes;
        v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory = ctx.capture_plane_mem_type;
        if (ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
            v4l2_buf.m.planes[0].m.fd = ctx.dmabuff_fd[i];
        ret = dec->capture_plane.qBuffer(v4l2_buf, NULL);
        TEST_ERROR(ret < 0, "Error Qing buffer at output plane", error);
    }
    cout << "Query and set capture successful" << endl;
    return;

error:
    if (error)
    {
        abort();
        cerr << "Error in " << __func__ << endl;
    }
}

void *decoder_h264::dec_capture_loop_fcn(void *arg)
{
    //context_t *ctx = (context_t *)arg;
    decoder_h264 *tmp = (decoder_h264 *)(arg);
    NvVideoDecoder *dec = tmp->ctx.dec;
    struct v4l2_event ev;
    int ret;

    cout << "Starting decoder capture loop thread" << endl;
    /* Need to wait for the first Resolution change event, so that
       the decoder knows the stream resolution and can allocate appropriate
       buffers when we call REQBUFS. */
    do
    {
        /* Refer ioctl VIDIOC_DQEVENT */
        ret = dec->dqEvent(ev, 50000);
        if (ret < 0)
        {
            if (errno == EAGAIN)
            {
                cerr << "Timed out waiting for first V4L2_EVENT_RESOLUTION_CHANGE"
                     << endl;
            }
            else
            {
                cerr << "Error in dequeueing decoder event" << endl;
            }
            tmp->abort();
            break;
        }
    } while ((ev.type != V4L2_EVENT_RESOLUTION_CHANGE) && !tmp->ctx.got_error);

    /* Received the resolution change event, now can do query_and_set_capture. */
    if (!tmp->ctx.got_error)
        tmp->query_and_set_capture();

    /* Exit on error or EOS which is signalled in main() */
    while (!(tmp->ctx.got_error || dec->isInError() || tmp->ctx.got_eos))
    {
        NvBuffer *dec_buffer;

        /* Check for Resolution change again.
           Refer ioctl VIDIOC_DQEVENT */
        ret = dec->dqEvent(ev, false);
        if (ret == 0)
        {
            switch (ev.type)
            {
            case V4L2_EVENT_RESOLUTION_CHANGE:
                tmp->query_and_set_capture();
                continue;
            }
        }

        /* Decoder capture loop */
        while (1)
        {
            struct v4l2_buffer v4l2_buf;
            struct v4l2_plane planes[MAX_PLANES];

            memset(&v4l2_buf, 0, sizeof(v4l2_buf));
            memset(planes, 0, sizeof(planes));
            v4l2_buf.m.planes = planes;

            /* Dequeue a filled buffer. */
            if (dec->capture_plane.dqBuffer(v4l2_buf, &dec_buffer, NULL, 0))
            {
                if (errno == EAGAIN)
                {
                    usleep(1000);
                }
                else
                {
                    tmp->abort();
                    cerr << "Error while calling dequeue at capture plane" << endl;
                }
                break;
            }

            /* If we need to write to file or display the buffer, give
               the buffer to video converter output plane instead of
               returning the buffer back to decoder capture plane. */
            if (tmp->ctx.out_file || (!tmp->ctx.disable_rendering && !tmp->ctx.stats))
            {
#ifndef USE_NVBUF_TRANSFORM_API
                NvBuffer *conv_buffer;
                struct v4l2_buffer conv_output_buffer;
                struct v4l2_plane conv_planes[MAX_PLANES];

                memset(&conv_output_buffer, 0, sizeof(conv_output_buffer));
                memset(conv_planes, 0, sizeof(conv_planes));
                conv_output_buffer.m.planes = conv_planes;

                /* Get an empty conv output plane buffer from conv_output_plane_buf_queue. */
                pthread_mutex_lock(&ctx.queue_lock);
                while (ctx.conv_output_plane_buf_queue->empty())
                {
                    pthread_cond_wait(&ctx.queue_cond, &ctx.queue_lock);
                }
                conv_buffer = ctx.conv_output_plane_buf_queue->front();
                ctx.conv_output_plane_buf_queue->pop();
                pthread_mutex_unlock(&ctx.queue_lock);

                conv_output_buffer.index = conv_buffer->index;
                if (ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
                    dec_buffer->planes[0].fd = ctx.dmabuff_fd[v4l2_buf.index];

                /* enqueue converter output plane buffer. */
                if (ctx.conv->output_plane.qBuffer(conv_output_buffer, dec_buffer) < 0)
                {
                    abort();
                    cerr << "Error while queueing buffer at converter output plane"
                         << endl;
                    break;
                }
#else
                /* Clip & Stitch can be done by adjusting rectangle. */
                NvBufferRect src_rect, dest_rect;
                src_rect.top = 0;
                src_rect.left = 0;
                src_rect.width = tmp->ctx.display_width;
                src_rect.height = tmp->ctx.display_height;
                dest_rect.top = 0;
                dest_rect.left = 0;
                dest_rect.width = tmp->ctx.display_width;
                dest_rect.height = tmp->ctx.display_height;

                NvBufferTransformParams transform_params;
                memset(&transform_params, 0, sizeof(transform_params));
                /* Indicates which of the transform parameters are valid. */
                transform_params.transform_flag = NVBUFFER_TRANSFORM_FILTER;
                transform_params.transform_flip = NvBufferTransform_None;
                transform_params.transform_filter = NvBufferTransform_Filter_Nearest;
                transform_params.src_rect = src_rect;
                transform_params.dst_rect = dest_rect;

                if (tmp->ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
                    dec_buffer->planes[0].fd = tmp->ctx.dmabuff_fd[v4l2_buf.index];
                /* Perform Blocklinear to PitchLinear conversion. */
                ret = NvBufferTransform(dec_buffer->planes[0].fd, tmp->ctx.dst_dma_fd, &transform_params);
                if (ret == -1)
                {
                    cerr << "Transform failed" << endl;
                    break;
                }

                if (!tmp->ctx.stats && !tmp->ctx.disable_rendering)
                {
                    tmp->ctx.renderer->render(tmp->ctx.dst_dma_fd);
                }

                if (tmp->ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
                    dec_buffer->planes[0].fd = tmp->ctx.dmabuff_fd[v4l2_buf.index];
                /* Perform Blocklinear to PitchLinear conversion. */
                ret = NvBufferTransform(dec_buffer->planes[0].fd, tmp->ctx.dst_dma_fd, &transform_params);
                if (ret == -1)
                {
                    cerr << "Transform failed" << endl;
                    break;
                }

                NvBufferParams parm;
                ret = NvBufferGetParams(tmp->ctx.dst_dma_fd, &parm);
                uint32_t buf1_len = 0, buf2_len = 0, buf3_len = 0;
                char *tp;
                if (tmp->buf1_use_flag == -1 || tmp->buf1_use_flag == 0)
                {
                    printf("buf1 flag use is %d\n", tmp->buf1_use_flag);
                    tp = tmp->buf1;
                }
                if (tmp->buf1_use_flag == 1)
                {
                    printf("buf1 flag use is %d\n", tmp->buf1_use_flag);
                    tp = tmp->buf1_shadow;
                }
                void *psrc_data;

                ret = NvBufferMemMap(tmp->ctx.dst_dma_fd, 0, NvBufferMem_Read_Write, &psrc_data);
                if (ret == 0)
                {
                    unsigned int i = 0;
                    NvBufferMemSyncForCpu(tmp->ctx.dst_dma_fd, 0, &psrc_data);
                    printf("height 0 is %d\n", parm.height[0]);
                    buf1_len = parm.height[0] * parm.width[0];
                    for (i = 0; i < parm.height[0]; ++i)
                    {
                        memcpy(&tp[i * parm.width[0]], (char *)psrc_data + i * parm.pitch[0], parm.width[0]);
                    }
                    NvBufferMemUnMap(tmp->ctx.dst_dma_fd, 0, &psrc_data);
                }
                ret = NvBufferMemMap(tmp->ctx.dst_dma_fd, 1, NvBufferMem_Read_Write, &psrc_data);
                if (ret == 0)
                {
                    unsigned int i = 0;
                    NvBufferMemSyncForCpu(tmp->ctx.dst_dma_fd, 1, &psrc_data);
                    printf("height 1 is %d\n", parm.height[1]);
                    buf2_len = parm.width[0] * parm.height[1];
                    for (i = 0; i < parm.height[1]; ++i)
                    {
                        memcpy(&tp[i * parm.width[0] + buf1_len], (char *)psrc_data + i * parm.pitch[1], parm.width[0]);
                    }
                    NvBufferMemUnMap(tmp->ctx.dst_dma_fd, 1, &psrc_data);
                }

                //
                //cv::Mat yuvimg(tmp->height * 3 / 2, tmp->width, CV_8UC1, tp);
                //cv::Mat rgbimg(tmp->height, tmp->width, CV_8UC3);
                //cvtColor(yuvimg, rgbimg, cv::COLOR_YUV2BGR_NV12);
                //tmp->mat_cache=rgbimg.clone();
                //cv::imwrite("test.jpg",rgbimg);
                /* If not writing to file, Queue the buffer back once it has been used. */
                if (tmp->ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
                    v4l2_buf.m.planes[0].m.fd = tmp->ctx.dmabuff_fd[v4l2_buf.index];
                if (dec->capture_plane.qBuffer(v4l2_buf, NULL) < 0)
                {
                    tmp->abort();
                    cerr << "Error while queueing buffer at decoder capture plane"
                         << endl;
                    break;
                }
#endif
            }
            else
            {
                /* If not writing to file, Queue the buffer back once it has been used. */
                if (tmp->ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
                    v4l2_buf.m.planes[0].m.fd = tmp->ctx.dmabuff_fd[v4l2_buf.index];
                if (dec->capture_plane.qBuffer(v4l2_buf, NULL) < 0)
                {
                    tmp->abort();
                    cerr << "Error while queueing buffer at decoder capture plane"
                         << endl;
                    break;
                }
            }
        }
    }
    cout << "Exiting decoder capture loop thread" << endl;
    return NULL;
}
bool decoder_h264::decoder_proc_blocking(context_t &ctx, bool eos, uint32_t current_file, int current_loop,
                                         char *nalu_parse_buffer)
{
    int allow_DQ = true;
    int ret = 0;
    struct v4l2_buffer temp_buf;

    /* Since all the output plane buffers have been queued, we first need to
       dequeue a buffer from output plane before we can read new data into it
       and queue it again. */
    while (!eos && !ctx.got_error && !ctx.dec->isInError())
    {
        struct v4l2_buffer v4l2_buf;
        struct v4l2_plane planes[MAX_PLANES];
        NvBuffer *buffer;

        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        memset(planes, 0, sizeof(planes));

        v4l2_buf.m.planes = planes;

        /* dequeue a buffer for output plane. */
        if (allow_DQ)
        {
            ret = ctx.dec->output_plane.dqBuffer(v4l2_buf, &buffer, NULL, -1);
            if (ret < 0)
            {
                cerr << "Error DQing buffer at output plane" << endl;
                abort();
                break;
            }
        }
        else
        {
            allow_DQ = true;
            memcpy(&v4l2_buf, &temp_buf, sizeof(v4l2_buffer));
            buffer = ctx.dec->output_plane.getNthBuffer(v4l2_buf.index);
        }

        /* read the input chunks. */
        read_decoder_input_chunk(ctx.in_file[current_file], buffer);

        v4l2_buf.m.planes[0].bytesused = buffer->planes[0].bytesused;

        if (ctx.input_nalu && ctx.copy_timestamp && ctx.flag_copyts)
        {
            /* Update the timestamp. */
            v4l2_buf.flags |= V4L2_BUF_FLAG_TIMESTAMP_COPY;
            ctx.timestamp += ctx.timestampincr;
            v4l2_buf.timestamp.tv_sec = ctx.timestamp / (MICROSECOND_UNIT);
            v4l2_buf.timestamp.tv_usec = ctx.timestamp % (MICROSECOND_UNIT);
        }

        /* enqueue a buffer for output plane. */
        ret = ctx.dec->output_plane.qBuffer(v4l2_buf, NULL);
        if (ret < 0)
        {
            cerr << "Error Qing buffer at output plane" << endl;
            abort();
            break;
        }
        if (v4l2_buf.m.planes[0].bytesused == 0)
        {
            eos = true;
            cout << "Input file read complete  2" << endl;
            break;
        }
    }
    return eos;
}
int decoder_h264::decode_proc()
{

    int ret = 0;
    int error = 0;
    uint32_t current_file = 0;
    uint32_t i;
    bool eos = false;
    int current_loop = 0;
    char *nalu_parse_buffer = NULL;
    NvApplicationProfiler &profiler = NvApplicationProfiler::getProfilerInstance();
    /* Set default values for decoder context members. */
    printf("before set default\n");
    set_defaults(&ctx);
    printf("set default over\n");
    char inpath[] = "m1.h264";
    printf("open in file\n");
    /* Open the input file. */
    ctx.in_file = (std::ifstream **)malloc(sizeof(std::ifstream *) * ctx.file_count);
    ctx.in_file[0] = new ifstream(inpath);
    /* Set thread name for decoder Output Plane thread. */
    pthread_setname_np(pthread_self(), "DecOutPlane");
    //other settings in parse argv
    ctx.out_file_path = NULL; //set output file path
    ctx.out_pixfmt = 1;       //1 NV12, 2 I420, 3 NV16, 4 NV24 [Default = 1]
    ctx.stats = false;        //Report profiling data for the app
    ctx.disable_rendering = false;
    ctx.disable_dpb = false;
    ctx.fullscreen = false;
    //ctx.window_height = 1080;
    //ctx->window_width = 1920;
    //ctx.window_x = 0;
    //ctx.window_y = 0;
    ctx.fps = 30;
    ctx.input_nalu = false;
    ctx.copy_timestamp = false;
    ctx.enable_metadata = false;
    ctx.enable_input_metadata = false;
    ctx.stress_test = 0;
    ctx.output_plane_mem_type = V4L2_MEMORY_MMAP; //1 = V4L2_MEMORY_MMAP, 2 = V4L2_MEMORY_USERPTR, Default = V4L2_MEMORY_MMAP
    ctx.max_perf = 1;                             //enable max performance
    ctx.extra_cap_plane_buffer = 1;               //32 max
    //ctx.skip_frames = 0;//0 = Decode all frames  1 = Skip non-reference frames  2 = Decode only key frames
    ctx.bLoop = false;
    ctx.bQueue = false;
    ctx.capture_plane_mem_type = V4L2_MEMORY_MMAP;
    ctx.blocking_mode = 1;

    /* Create NvVideoDecoder object for blocking or non-blocking I/O mode. */
    cout << "Creating decoder in blocking mode \n";
    ctx.dec = NvVideoDecoder::createVideoDecoder("dec0");
    cout << "set decode over\n"
         << endl;
    /* Subscribe to Resolution change event.
       Refer ioctl VIDIOC_SUBSCRIBE_EVENT */
    ret = ctx.dec->subscribeEvent(V4L2_EVENT_RESOLUTION_CHANGE, 0, 0);
    /* Set format on the output plane.
       Refer ioctl VIDIOC_S_FMT */
    ret = ctx.dec->setOutputPlaneFormat(ctx.decoder_pixfmt, CHUNK_SIZE);
    /* Configure for frame input mode for decoder.
       Refer V4L2_CID_MPEG_VIDEO_DISABLE_COMPLETE_FRAME_INPUT */
    /* Input to the decoder will be a chunk of bytes.
    NOTE: Set V4L2_CID_MPEG_VIDEO_DISABLE_COMPLETE_FRAME_INPUT control to
    false so that application can send chunks of encoded data instead
    of forming complete frames. */
    printf("Setting frame input mode to 1 \n");
    ret = ctx.dec->setFrameInputMode(1);
    /* Disable decoder DPB management.
       NOTE: V4L2_CID_MPEG_VIDEO_DISABLE_DPB should be set after output plane
             set format */
    if (ctx.disable_dpb)
    {
        ret = ctx.dec->disableDPB();
    }
    /* Enable decoder error and metadata reporting.
       Refer V4L2_CID_MPEG_VIDEO_ERROR_REPORTING */
    if (ctx.enable_metadata || ctx.enable_input_metadata)
    {
        ret = ctx.dec->enableMetadataReporting();
        TEST_ERROR(ret < 0, "Error while enabling metadata reporting", cleanup);
    }

    /* Enable max performance mode by using decoder max clock settings.
       Refer V4L2_CID_MPEG_VIDEO_MAX_PERFORMANCE */
    if (ctx.max_perf)
    {
        ret = ctx.dec->setMaxPerfMode(ctx.max_perf);
        TEST_ERROR(ret < 0, "Error while setting decoder to max perf", cleanup);
    }

    /* Set the skip frames property of the decoder.
       Refer V4L2_CID_MPEG_VIDEO_SKIP_FRAMES */
    if (ctx.skip_frames)
    {
        ret = ctx.dec->setSkipFrames(ctx.skip_frames);
        TEST_ERROR(ret < 0, "Error while setting skip frames param", cleanup);
    }

    /* Query, Export and Map the output plane buffers so can read
       encoded data into the buffers. */
    if (ctx.output_plane_mem_type == V4L2_MEMORY_MMAP)
    {
        /* configure decoder output plane for MMAP io-mode.
           Refer ioctl VIDIOC_REQBUFS, VIDIOC_QUERYBUF and VIDIOC_EXPBUF */
        ret = ctx.dec->output_plane.setupPlane(V4L2_MEMORY_MMAP, 2, true, false);
    }
    else if (ctx.output_plane_mem_type == V4L2_MEMORY_USERPTR)
    {
        /* configure decoder output plane for USERPTR io-mode.
           Refer ioctl VIDIOC_REQBUFS */
        ret = ctx.dec->output_plane.setupPlane(V4L2_MEMORY_USERPTR, 10, false, true);
    }
    TEST_ERROR(ret < 0, "Error while setting up output plane", cleanup);

    /* Start stream processing on decoder output-plane.
   Refer ioctl VIDIOC_STREAMON */
    ret = ctx.dec->output_plane.setStreamStatus(true);
    TEST_ERROR(ret < 0, "Error in output plane stream on", cleanup);

    /* Enable copy timestamp with start timestamp in seconds for decode fps.
   NOTE: Used to demonstrate how timestamp can be associated with an
         individual H264/H265 frame to achieve video-synchronization. */
    if (ctx.copy_timestamp && ctx.input_nalu)
    {
        ctx.timestamp = (ctx.start_ts * MICROSECOND_UNIT);
        ctx.timestampincr = (MICROSECOND_UNIT * 16) / ((uint32_t)(ctx.dec_fps * 16));
    }
    /* Read encoded data and enqueue all the output plane buffers.
       Exit loop in case file read is complete. */
    i = 0;
    current_loop = 1;
    while (!eos && !ctx.got_error && !ctx.dec->isInError() &&
           i < ctx.dec->output_plane.getNumBuffers())
    {
        struct v4l2_buffer v4l2_buf;
        struct v4l2_plane planes[MAX_PLANES];
        NvBuffer *buffer;

        memset(&v4l2_buf, 0, sizeof(v4l2_buf));
        memset(planes, 0, sizeof(planes));

        buffer = ctx.dec->output_plane.getNthBuffer(i);
        /* read the input chunks. */
        read_decoder_input_chunk(ctx.in_file[current_file], buffer);

        v4l2_buf.index = i;
        v4l2_buf.m.planes = planes;
        v4l2_buf.m.planes[0].bytesused = buffer->planes[0].bytesused;
        /* It is necessary to queue an empty buffer to signal EOS to the decoder
       i.e. set v4l2_buf.m.planes[0].bytesused = 0 and queue the buffer. */
        ret = ctx.dec->output_plane.qBuffer(v4l2_buf, NULL);
        if (ret < 0)
        {
            cerr << "Error Qing buffer at output plane" << endl;
            abort();
            break;
        }
        if (v4l2_buf.m.planes[0].bytesused == 0)
        {
            eos = true;
            cout << "Input file read complete  3" << endl;
            break;
        }
        i++;
    }
    /* Create threads for decoder output */
    if (ctx.blocking_mode)
    {
        pthread_create(&ctx.dec_capture_loop, NULL, dec_capture_loop_fcn, this);
        /* Set thread name for decoder Capture Plane thread. */
        pthread_setname_np(ctx.dec_capture_loop, "DecCapPlane");
    }

    if (ctx.blocking_mode)
        eos = decoder_proc_blocking(ctx, eos, current_file, current_loop, nalu_parse_buffer);
    /* After sending EOS, all the buffers from output plane should be dequeued.
   and after that capture plane loop should be signalled to stop. */
    if (ctx.blocking_mode)
    {
        while (ctx.dec->output_plane.getNumQueuedBuffers() > 0 &&
               !ctx.got_error && !ctx.dec->isInError())
        {
            struct v4l2_buffer v4l2_buf;
            struct v4l2_plane planes[MAX_PLANES];

            memset(&v4l2_buf, 0, sizeof(v4l2_buf));
            memset(planes, 0, sizeof(planes));

            v4l2_buf.m.planes = planes;
            ret = ctx.dec->output_plane.dqBuffer(v4l2_buf, NULL, NULL, -1);
            if (ret < 0)
            {
                cerr << "Error DQing buffer at output plane" << endl;
                abort();
                break;
            }
        }
    }

    /* Signal EOS to the decoder capture loop. */
    ctx.got_eos = true;
cleanup:
    if (ctx.blocking_mode && ctx.dec_capture_loop)
    {
        pthread_join(ctx.dec_capture_loop, NULL);
    }
    if (ctx.capture_plane_mem_type == V4L2_MEMORY_DMABUF)
    {
        for (int index = 0; index < ctx.numCapBuffers; index++)
        {
            if (ctx.dmabuff_fd[index] != 0)
            {
                ret = NvBufferDestroy(ctx.dmabuff_fd[index]);
                if (ret < 0)
                {
                    cerr << "Failed to Destroy NvBuffer" << endl;
                }
            }
        }
    }
    if (ctx.dec && ctx.dec->isInError())
    {
        cerr << "Decoder is in error" << endl;
        error = 1;
    }

    if (ctx.got_error)
    {
        error = 1;
    }

    /* The decoder destructor does all the cleanup i.e set streamoff on output and
       capture planes, unmap buffers, tell decoder to deallocate buffer (reqbufs
       ioctl with count = 0), and finally call v4l2_close on the fd. */
    delete ctx.dec;
    /* Similarly, EglRenderer destructor does all the cleanup. */
    delete ctx.renderer;
    for (uint32_t i = 0; i < ctx.file_count; i++)
        delete ctx.in_file[i];
    printf("delete in file over \n");
#ifndef USE_NVBUF_TRANSFORM_API
    delete ctx.conv_output_plane_buf_queue;
#else
    if (ctx.dst_dma_fd != -1)
    {
        NvBufferDestroy(ctx.dst_dma_fd);
        ctx.dst_dma_fd = -1;
    }
#endif
    delete[] nalu_parse_buffer;
    free(ctx.in_file);
    return -error;
}

void decoder_h264::stream_in_data(uint8_t *data, uint32_t len)
{
    if (len > 0)
    {
        char *data1 = (char *)malloc(len);
        memcpy(data1, data, len);
        CQ tmpq;
        tmpq.data = data1;
        tmpq.len = len;
        cq_queue.push(tmpq);
    }
    return;
}

int decoder_h264::get_mat(cv::Mat *op)
{
    int mutex_flag = pthread_mutex_trylock(&clone_mutex);
    if (mutex_flag == 0)
    {
        *op = mat_cache.clone();
        pthread_mutex_unlock(&clone_mutex);
        printf("get mat success+++++++++\n");
        return 0;
    }
    else
    {
        printf("get mat failed-----------\n");
        return -1;
    }
}

void *decoder_h264::image_save_thread(void *__this)
{
    decoder_h264 *tmp = (decoder_h264 *)(__this);
    int counter = 0;
    while (1)
    {
        usleep(50000);
        cv::Mat tmp_op;
        tmp->get_mat(&tmp_op);
        cv::imwrite("test" + to_string(counter) + ".jpg", tmp_op);
        counter++;
    }
    return nullptr;
}

void *decoder_h264::image_cvt(void *__this)
{
    decoder_h264 *tmp = (decoder_h264 *)(__this);
    while (1)
    {
        usleep(50000);
        if (tmp->buf1_use_flag == -1 || tmp->buf1_use_flag == 0)
        {
            tmp->buf1_use_flag = 1;
            cv::Mat yuvimg(tmp->height * 3 / 2, tmp->width, CV_8UC1, tmp->buf1);
            cv::Mat rgbimg(tmp->height, tmp->width, CV_8UC3);
            cvtColor(yuvimg, rgbimg, cv::COLOR_YUV2BGR_NV12);
            int mutex_flag = pthread_mutex_trylock(&tmp->clone_mutex);
            if (mutex_flag == 0)
            {
                tmp->mat_cache = rgbimg.clone();
                pthread_mutex_unlock(&tmp->clone_mutex);
            }
        }
        else if (tmp->buf1_use_flag == 1)
        {
            tmp->buf1_use_flag = 0;
            cv::Mat yuvimg(tmp->height * 3 / 2, tmp->width, CV_8UC1, tmp->buf1_shadow);
            cv::Mat rgbimg(tmp->height, tmp->width, CV_8UC3);
            cvtColor(yuvimg, rgbimg, cv::COLOR_YUV2BGR_NV12);
            int mutex_flag = pthread_mutex_trylock(&tmp->clone_mutex);
            if (mutex_flag == 0)
            {
                tmp->mat_cache = rgbimg.clone();
                pthread_mutex_unlock(&tmp->clone_mutex);
            }
        }
    }
    return nullptr;
}
