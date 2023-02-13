#include <gst/gst.h>
#include <string.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// To compile, use: gcc working-rx.c -o working-rx `pkg-config --cflags --libs gstreamer-1.0` -lm 
int
working_main (int argc, char *argv[])
{
  GstElement *pipeline, *source, *sink, * converter , *decode_bin;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;
  char *file_source_location;
  
  if(argc!=2){
		g_printerr("Give input in format:%s filename, or %s screen\n", argv[0],argv[0]);
		return -1;	
	}

  file_source_location = argv[1];

  g_printerr("file source: %s", file_source_location);

  /* Initialize GStreamer */
  gst_init (&argc, &argv);

  /* Create the elements */
  // source = gst_element_factory_make (usb_driver_name, "source");
  source = gst_element_factory_make ("filesrc", "source");

/*  if(strcmp(file_source_location, "screen") == 0 ){
    sink = gst_element_factory_make ("autovideosink", "sink");
  }
  else{
    sink = gst_element_factory_make ("filesink", "sink");
  }
  */

  sink = gst_element_factory_make ("autovideosink", "sink");

  decode_bin = gst_element_factory_make ("decodebin", "decodebin");

  g_object_set (source, "location", file_source_location, "typefind", TRUE, NULL);

  converter = gst_element_factory_make ("videoconvert", "converter");

  /* Create the empty pipeline */
  pipeline = gst_pipeline_new ("test-pipeline");
  
  if (!pipeline || !source || !sink || !converter || !decode_bin) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }

  /* Build the pipeline */
  gst_bin_add_many (GST_BIN (pipeline), source,  decode_bin, sink, NULL);
  if (gst_element_link_many (source, decode_bin, converter, sink, NULL) != TRUE) {
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