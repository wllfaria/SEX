# SEX

> Spotify EXperience Tracker.

SEX is a daemon that tracks playerctl status and compute the total minutes of
active time. Which is store inside a file on a path you can chose.


# How to setup

First, you must open sex.c and change `SEX_PATH` to be the ABSOLUTE path where
you want to store the file with the session data. This step is really important,
and it has to be a absolute path.

```c
// example
#define SEX_PATH "/home/user/some_directory/sex_diary"
```

# How to build

Building SEX is as simple as running `make sex` on the root directory. This will
produce a binary inside of the `build/` directory. which you can then use as 
you'd like.


If you're using I3. you can setup it to be auto executed on startup with the 
following commands:

```bash
mv build/sex ~/.config/i3/sex

# Inside of .config/i3/config add the following:
exec_always --no-startup-id ~/.config/i3/sex
```
