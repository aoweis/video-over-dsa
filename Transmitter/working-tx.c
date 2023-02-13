#include <gst/gst.h>
#include <string.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// To compile, use: gcc working-tx.c -o working-tx `pkg-config --cflags --libs gstreamer-1.0` -lm 
int
working_main (int argc, char *argv[])
{
  GstElement *pipeline, *source, *sink, * converter;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  char *file_sink_location;
  char *usb_driver_name;

  if(argc!=3){
		g_printerr("Give input in format:%s filename usb_driver_name, or %s screen usb_driver_name\n", argv[0],argv[0]);
		return -1;	
	}

  file_sink_location = argv[1];
  usb_driver_name = argv[2];

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  source = gst_element_factory_make (usb_driver_name, "source");

  if(strcmp(file_sink_location, "screen") == 0 ){
    sink = gst_element_factory_make ("autovideosink", "sink");
  }
  else{
    sink = gst_element_factory_make ("filesink", "sink");
  }
  
  g_object_set (sink, "location", file_sink_location, NULL);

  converter = gst_element_factory_make ("videoconvert", "converter");

  /* Create the empty pipeline */
  pipeline = gst_pipeline_new ("test-pipeline");

    if (!pipeline || !source || !sink || !converter) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  /* Build the pipeline */
gst_bin_add_many (GST_BIN (pipeline), source,  converter, sink, NULL);
  if (gst_element_link_many (source, converter, sink, NULL) != TRUE) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* Start playing */
  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE) {
    g_printerr ("Unable to set the pipeline to the playing state.\n");
    gst_object_unref (pipeline);
    return -1;
  }

  /* Wait until error or EOS */
  bus = gst_element_get_bus (pipeline);
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Parse message */
  if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
      case GST_MESSAGE_ERROR:
        gst_message_parse_error (msg, &err, &debug_info);
        g_printerr ("Error received from element %s: %s\n",
            GST_OBJECT_NAME (msg->src), err->message);
        g_printerr ("Debugging information: %s\n",
            debug_info ? debug_info : "none");
        g_clear_error (&err);
        g_free (debug_info);
        break;
      case GST_MESSAGE_EOS:
        g_print ("End-Of-Stream reached.\n");
        break;
      default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        g_printerr ("Unexpected message received.\n");
        break;
    }
    gst_message_unref (msg);
  }

  /* Free resources */
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}

int
main (int argc, char *argv[])
{
#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
  return gst_macos_main (tutorial_main, argc, argv, NULL);
#else
  return working_main (argc, argv);
#endif
}