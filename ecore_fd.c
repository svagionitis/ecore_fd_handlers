#include <Ecore.h>
#include <unistd.h>
#include <string.h>

struct context
{
   Ecore_Fd_Handler *handler;
   Ecore_Timer      *timer;
};

static void
_fd_prepare_cb(void *data, Ecore_Fd_Handler *handler)
{
   printf("prepare_cb called.\n");
}

static Eina_Bool
_fd_read_handler_cb(void *data, Ecore_Fd_Handler *handler)
{
   struct context *ctxt = data;
   char buf[1024];
   size_t nbytes;
   int fd;

   printf("_fd_read_handler_cb\n");

   if (ecore_main_fd_handler_active_get(handler, ECORE_FD_ERROR))
     {
        printf("An error has occurred. Stop watching this fd and quit.\n");
        ecore_main_loop_quit();
        ctxt->handler = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   fd = ecore_main_fd_handler_fd_get(handler);
   nbytes = read(fd, buf, sizeof(buf));
   if (nbytes == 0)
     {
        printf("_fd_read_handler_cb: Nothing to read, exiting...\n");
        ecore_main_loop_quit();
        ctxt->handler = NULL;
        return ECORE_CALLBACK_CANCEL;
     }
   buf[nbytes - 1] = '\0';

   printf("Read %zd bytes from input: \"%s\"\n", nbytes - 1, buf);

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_fd_fgets_handler_cb(void *data, Ecore_Fd_Handler *handler)
{
   struct context *ctxt = data;
   char buf[1024];
   size_t nbytes;
   int fd;

   printf("_fd_fgets_handler_cb\n");

   if (ecore_main_fd_handler_active_get(handler, ECORE_FD_ERROR))
     {
        printf("An error has occurred. Stop watching this fd and quit.\n");
        ecore_main_loop_quit();
        ctxt->handler = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   fd = ecore_main_fd_handler_fd_get(handler);
   if (fd == fileno(stdin))
      {
          if (fgets(buf, sizeof(buf), stdin) == NULL)
             {
                printf("_fd_fgets_handler_cb: Nothing to read, exiting...\n");
                ecore_main_loop_quit();
                ctxt->handler = NULL;
                return ECORE_CALLBACK_CANCEL;
             }
          nbytes = strlen(buf);
      }
//   buf[nbytes - 1] = '\0';

   printf("Read %zd bytes from input: \"%s\"\n", nbytes, buf);

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_fd_getline_handler_cb(void *data, Ecore_Fd_Handler *handler)
{
   struct context *ctxt = data;
   char *cli_line = NULL;
   size_t cli_len = 0;
   ssize_t read_bytes = 0;
   int fd;

   printf("_fd_getline_handler_cb\n");

   if (ecore_main_fd_handler_active_get(handler, ECORE_FD_ERROR))
     {
        printf("An error has occurred. Stop watching this fd and quit.\n");
        ecore_main_loop_quit();
        ctxt->handler = NULL;
        return ECORE_CALLBACK_CANCEL;
     }

   fd = ecore_main_fd_handler_fd_get(handler);
   if (fd == fileno(stdin))
     {
         if ((read_bytes = getline(&cli_line, &cli_len, stdin)) == -1)
           {
              printf("_fd_getline_handler_cb: Nothing to read, exiting...\n");
              ecore_main_loop_quit();
              ctxt->handler = NULL;
              return ECORE_CALLBACK_CANCEL;
           }
     }

   printf("Read %zd bytes (Max: %zd) from input: \"%s\"\n", read_bytes, cli_len, cli_line);

   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
_timer_cb(void *data)
{
   printf("Timer expired after 1 second(s)...\n");

   return ECORE_CALLBACK_RENEW;
}

int
main(int argc, char **argv)
{
   struct context ctxt = {0};

   if (argc != 2)
     {
        printf("Usage: %s <choose callback function:read, getline, fgets>\n", argv[0]);
        return -1;
     }

   if (!ecore_init())
//   if (!ecore_evas_init())
     {
        printf("ERROR: Cannot init Ecore!\n");
        return -1;
     }

   /* Choose which callback to use*/
   if (strncmp(argv[1], "read", strlen(argv[1])) == 0)
     {
        ctxt.handler = ecore_main_fd_handler_add(STDIN_FILENO,
                                            ECORE_FD_READ | ECORE_FD_ERROR,
                                            _fd_read_handler_cb,
                                            &ctxt, NULL, NULL);
     }
   else if (strncmp(argv[1], "getline", strlen(argv[1])) == 0)
     {
        ctxt.handler = ecore_main_fd_handler_add(STDIN_FILENO,
                                            ECORE_FD_READ | ECORE_FD_ERROR,
                                            _fd_getline_handler_cb,
                                            &ctxt, NULL, NULL);
     }
   else if (strncmp(argv[1], "fgets", strlen(argv[1])) == 0)
     {
        ctxt.handler = ecore_main_fd_handler_add(STDIN_FILENO,
                                            ECORE_FD_READ | ECORE_FD_ERROR,
                                            _fd_fgets_handler_cb,
                                            &ctxt, NULL, NULL);
     }
   else
     {
        printf("Your input \"%s\" \n", argv[1]);
        return -1;
     }

//   ecore_main_fd_handler_prepare_callback_set(ctxt.handler, _fd_prepare_cb, &ctxt);
//   ctxt.timer = ecore_timer_add(1, _timer_cb, &ctxt);

   printf("Starting the main loop. Type anything and hit <enter> to "
          "activate the fd_%s_handler callback, or CTRL+d to shutdown.\n", argv[1]);

   ecore_main_loop_begin();

   if (ctxt.handler)
     ecore_main_fd_handler_del(ctxt.handler);

   if (ctxt.timer)
     ecore_timer_del(ctxt.timer);

   ecore_shutdown();
//   ecore_evas_shutdown();

   return 0;
}

