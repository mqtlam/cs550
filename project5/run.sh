for f in videos_/*.avi; do env LD_LIBRARY_PATH=/usr/local/lib:../edu.osu.vision ./main "$f"; done

