/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;

struct buffer {
        void *                  start;
        size_t                  length;
};

static char *           dev_name        = NULL;
static io_method	io		= IO_METHOD_MMAP;	// 目前設定為mmap的方式
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

static void
errno_exit                      (const char *           s)
{
        fprintf (stderr, "%s error %d, %s\n",
                 s, errno, strerror (errno));

        exit (EXIT_FAILURE);
}

static int
xioctl                          (int                    fd,
                                 int                    request,
                                 void *                 arg)
{
        int r;

        do
	{
		r = ioctl (fd, request, arg);	// 對fd裝置下系統命令
        }while (-1 == r && EINTR == errno);	// 表示是系統呼叫，但是，回傳錯誤(-1)，必需再執行一次，可能正常busy之類
						// 被其它應用程式利用系統呼叫執行中斷

        return r;
}

static void
process_image                   (const void *           p)
{
        fputc ('.', stdout);
        fflush (stdout);	// 把buffer內的資料清出到螢幕上
}

static int
read_frame			(void)	// 讀取frame
{
        struct v4l2_buffer buf;
	unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
    		if (-1 == read (fd, buffers[0].start, buffers[0].length)) {	// 這裡不懂，不是buffer這一個指標就是[0]這一個位置嗎?為什麼去掉中括號就不行?
            		switch (errno) {
            		case EAGAIN:
                    		return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("read");
			}
		}

    		process_image (buffers[0].start);

		break;

	case IO_METHOD_MMAP:
		CLEAR (buf);	// 清除buf內的所有資料

            	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	// 設定buf的型態,同時決定怎麼處理這一個buffer
            	buf.memory = V4L2_MEMORY_MMAP;		// 設定memory map的型態

		// Applications call the VIDIOC_DQBUF ioctl to dequeue a filled (capturing) or displayed (output) buffer from the driver's outgoing queue.
    		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
            		switch (errno) {
            		case EAGAIN:	// Try again
                    		return 0;

			case EIO:	// I/O error
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("VIDIOC_DQBUF");
			}
		}

                assert (buf.index < n_buffers);	// 除錯用的:只能接受buf.index小於n_buffers,表明有個嚴重的程序錯誤

	        process_image (buffers[buf.index].start);	// 執行100次的地方

		// Applications call the VIDIOC_QBUF ioctl to enqueue an empty (capturing) or filled (output) buffer in the driver's incoming queue. The semantics depend on the selected I/O method.
		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))	// 把資料存放到queue中??
			errno_exit ("VIDIOC_QBUF");

		break;

	case IO_METHOD_USERPTR:
		CLEAR (buf);

    		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    		buf.memory = V4L2_MEMORY_USERPTR;

		if (-1 == xioctl (fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				return 0;

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:
				errno_exit ("VIDIOC_DQBUF");
			}
		}

		for (i = 0; i < n_buffers; ++i)
			if (buf.m.userptr == (unsigned long) buffers[i].start
			    && buf.length == buffers[i].length)
				break;

		assert (i < n_buffers);

    		process_image ((void *) buf.m.userptr);

		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
			errno_exit ("VIDIOC_QBUF");

		break;
	}

	return 1;
}

static void
mainloop                        (void)
{
	unsigned int count;

        count = 100;

        while (count-- > 0) {
                for (;;) {
                        fd_set fds;
                        struct timeval tv;	// 用來執行timeout()
                        int r;

                        FD_ZERO (&fds);		// 清空fds與所有文件描述值的關聯
                        FD_SET (fd, &fds);	// 加入fd file descripter到file descripter set中

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select (fd + 1, &fds, NULL, NULL, &tv);	// 直到發生fds或是時間tv發生timeout

                        if (-1 == r) {	// 發生錯誤
                                if (EINTR == errno)
                                        continue;

                                errno_exit ("select");
                        }

                        if (0 == r) {	// 時間到了仍沒有設備準備好
                                fprintf (stderr, "select timeout\n");
                                exit (EXIT_FAILURE);
                        }

			if (read_frame ())
                    		break;
	
			/* EAGAIN - continue select loop. */
                }
        }
}

static void
stop_capturing                  (void)
{
        enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMOFF, &type))
			errno_exit ("VIDIOC_STREAMOFF");

		break;
	}
}

static void
start_capturing                 (void)
{
        unsigned int i;
        enum v4l2_buf_type type;

	switch (io) {
	case IO_METHOD_READ:
		/* Nothing to do. */
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
            		struct v4l2_buffer buf;	// 準備把資料存放到buffer中

        		CLEAR (buf);

        		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        		buf.memory      = V4L2_MEMORY_MMAP;
        		buf.index       = i;

        		if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))	// Exchange a buffer with the driver
                    		errno_exit ("VIDIOC_QBUF");
		}
		
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))	// Start streaming I/O
			errno_exit ("VIDIOC_STREAMON");

		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {	// 共有n_buffers個frame??
            		struct v4l2_buffer buf;

        		CLEAR (buf);	// 準備把資料存到這一個buffer中

        		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        		buf.memory      = V4L2_MEMORY_USERPTR;
			buf.index       = i;
			buf.m.userptr	= (unsigned long) buffers[i].start;
			buf.length      = buffers[i].length;

			if (-1 == xioctl (fd, VIDIOC_QBUF, &buf))
                    		errno_exit ("VIDIOC_QBUF");
		}

		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

		if (-1 == xioctl (fd, VIDIOC_STREAMON, &type))
			errno_exit ("VIDIOC_STREAMON");

		break;
	}
}

static void
uninit_device                   (void)
{
        unsigned int i;

	switch (io) {
	case IO_METHOD_READ:
		free (buffers[0].start);
		break;

	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap (buffers[i].start, buffers[i].length))
				errno_exit ("munmap");
		break;

	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
			free (buffers[i].start);
		break;
	}

	free (buffers);
}

static void
init_read			(unsigned int		buffer_size)
{
        buffers = calloc (1, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

	buffers[0].length = buffer_size;
	buffers[0].start = malloc (buffer_size);

	if (!buffers[0].start) {
    		fprintf (stderr, "Out of memory\n");
            	exit (EXIT_FAILURE);
	}
}

static void
init_mmap			(void)
{
	struct v4l2_requestbuffers req;

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;	// 設定記憶體印射位置

	if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {	// Initiate Memory Mapping or User Pointer I/O
                if (EINVAL == errno) {
                        fprintf (stderr, "%s does not support "
                                 "memory mapping\n", dev_name);
                        exit (EXIT_FAILURE);
                } else {
                        errno_exit ("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf (stderr, "Insufficient buffer memory on %s\n",
                         dev_name);
                exit (EXIT_FAILURE);
        }

        buffers = calloc (req.count, sizeof (*buffers));

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");
                exit (EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;	// 存放讀取進來的資料的buffer

                CLEAR (buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl (fd, VIDIOC_QUERYBUF, &buf))	// Query the status of a buffer
                        errno_exit ("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap (NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit ("mmap");
        }
}

static void
init_userp			(unsigned int		buffer_size)
{
	struct v4l2_requestbuffers req;
        unsigned int page_size;

        page_size = getpagesize ();
        buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl (fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf (stderr, "%s does not support "
                                 "user pointer i/o\n", dev_name);
                        exit (EXIT_FAILURE);
                } else {
                        errno_exit ("VIDIOC_REQBUFS");
                }
        }

        buffers = calloc (4, sizeof (*buffers));	// 設定4個buffers大小的空間

        if (!buffers) {
                fprintf (stderr, "Out of memory\n");	// 分配不到空間啦
                exit (EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = memalign (/* boundary */ page_size,	// 配置buffer_size的空間,且啟始位址必定為page_size的倍數,若return 為0,則表示出現錯誤
                                                     buffer_size);

                if (!buffers[n_buffers].start) {
    			fprintf (stderr, "Out of memory\n");
            		exit (EXIT_FAILURE);
		}
        }
}

static void
init_device                     (void)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
	unsigned int min;

        if (-1 == xioctl (fd, VIDIOC_QUERYCAP, &cap)) {	// 對device下命令. VIDIOC_QUERYCAP:想要了解device的能力,把結果存到v4l2_capability的變數中
                if (EINVAL == errno) {	// Invalid argumenfpt
                        fprintf (stderr, "%s is no V4L2 device\n",
                                 dev_name);
                        exit (EXIT_FAILURE);
                } else {
                        errno_exit ("VIDIOC_QUERYCAP");	// 表示是在VIDIOC_QUERYCAP時出現錯誤
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {	// 了解是否有V4L2_CAP_VIDEO_CAPTURE的功能
                fprintf (stderr, "%s is no video capture device\n",
                         dev_name);
                exit (EXIT_FAILURE);
        }

	switch (io) {
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {	// 了解是否有支援V4L2_CAP_READWRITE的功能
			fprintf (stderr, "%s does not support read i/o\n",
				 dev_name);
			exit (EXIT_FAILURE);
		}

		break;

	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {	// 了解是否有支援V4L2_CAP_STREAMING的功能
			fprintf (stderr, "%s does not support streaming i/o\n",
				 dev_name);
			exit (EXIT_FAILURE);
		}

		break;
	}


        /* Select video input, video standard and tune here. */


	CLEAR (cropcap);	// 清除cropcap的內容值

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;	// 設定擷取buffer的型態

        if (0 == xioctl (fd, VIDIOC_CROPCAP, &cropcap)) {	// 對device執行擷取的動作??
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl (fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:	// Invalid argumenfpt
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {	
                /* Errors ignored. */
        }


        CLEAR (fmt);

        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;	// 型態
        fmt.fmt.pix.width       = 640; 	// 寬
        fmt.fmt.pix.height      = 480;	// 高
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;		// 設定格式
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;	// both fields interlaced

        if (-1 == xioctl (fd, VIDIOC_S_FMT, &fmt))	// 是儲存檔案嗎?
                errno_exit ("VIDIOC_S_FMT");

        /* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)	// 設定寬度
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;	// 設定整個畫面的大小
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io) {
	case IO_METHOD_READ:
		init_read (fmt.fmt.pix.sizeimage);
		break;

	case IO_METHOD_MMAP:
		init_mmap ();
		break;

	case IO_METHOD_USERPTR:
		init_userp (fmt.fmt.pix.sizeimage);
		break;
	}
}

static void
close_device                    (void)
{
        if (-1 == close (fd))
	        errno_exit ("close");

        fd = -1;
}

static void
open_device                     (void)
{
        struct stat st; 

        if (-1 == stat (dev_name, &st)) {	// 讀取device的狀態,把狀態存到st
                fprintf (stderr, "Cannot identify '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit (EXIT_FAILURE);
        }

        if (!S_ISCHR (st.st_mode)) {	// 判斷是否為char device
                fprintf (stderr, "%s is no device\n", dev_name);
                exit (EXIT_FAILURE);
        }

        fd = open (dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);	// 打開標準輔助設備,且設定為nonblock

        if (-1 == fd) {
                fprintf (stderr, "Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror (errno));
                exit (EXIT_FAILURE);
        }
}

static void
usage                           (FILE *                 fp,
                                 int                    argc,
                                 char **                argv)
{
        fprintf (fp,
                 "Usage: %s [options]\n\n"
                 "Options:\n"
                 "-d | --device name   Video device name [/dev/video0]\n"
                 "-h | --help          Print this message\n"
                 "-m | --mmap          Use memory mapped buffers\n"
                 "-r | --read          Use read() calls\n"
                 "-u | --userp         Use application allocated buffers\n"
                 "",
		 argv[0]);
}

static const char short_options [] = "d:hmru";

static const struct option
long_options [] = {
        { "device",     required_argument,      NULL,           'd' },
        { "help",       no_argument,            NULL,           'h' },
        { "mmap",       no_argument,            NULL,           'm' },
        { "read",       no_argument,            NULL,           'r' },
        { "userp",      no_argument,            NULL,           'u' },
        { 0, 0, 0, 0 }
};

int
main                            (int                    argc,
                                 char **                argv)
{
        dev_name = "/dev/video0";

        for (;;) {
                int index;
                int c;
                
                c = getopt_long (argc, argv,
                                 short_options, long_options,
                                 &index);

                if (-1 == c)
                        break;

                switch (c) {
                case 0: /* getopt_long() flag */
                        break;

                case 'd':
                        dev_name = optarg;
                        break;

                case 'h':
                        usage (stdout, argc, argv);
                        exit (EXIT_SUCCESS);

                case 'm':
                        io = IO_METHOD_MMAP;	// 使用mmap,而不是打開檔案來傳資料,這樣速度會比較快
			break;

                case 'r':
                        io = IO_METHOD_READ;
			break;

                case 'u':
                        io = IO_METHOD_USERPTR;
			break;

                default:
                        usage (stderr, argc, argv);
                        exit (EXIT_FAILURE);
                }
        }

        open_device ();

        init_device ();

        start_capturing ();

        mainloop ();

        stop_capturing ();

        uninit_device ();

        close_device ();

        exit (EXIT_SUCCESS);

        return 0;
}
