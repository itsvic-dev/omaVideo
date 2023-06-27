# frame commands:
## movement:
# move to X,Y pos - 0x01 [X, 16-bit word] [Y, 16-bit word]
# increment index by [n] - 0x02 [n, 8-bit word]
# increment index by 1 - 0x03
## drawing:
# draw color at current index - 0x10 [color, 8-bit word]
# invert color at current index - 0x11
# fill canvas with single color - 0x12 [color, 8-bit word]
# fill canvas with image data - 0x13 [color*, W*H sequence of 8-bit words]
## note: 0x10 and 0x11 commands both increment the index

CMD_MOVE = 0x01
CMD_INC_BY = 0x02
CMD_INC = 0x03

CMD_DRAW = 0x10
CMD_INVERT = 0x11
CMD_FILL = 0x12
CMD_FILL_DATA = 0x13


# video object:
# "OMAVIDEO" - magic header
# width, 16-bit word
# height, 16-bit word
# FPS, 8-bit word
# frame count, 16-bit word
# frames* - n-sequence of frame objects

# frame object:
# commands_length, 32-bit word
# commands* - sequence of frame commands with a total size of commands_length

from PIL import Image
import struct
import tqdm


def img_to_frame(prev_frame: Image.Image | None, img: Image.Image):
    commands = []
    if not prev_frame:
        # fill with black
        commands.append(CMD_FILL)
        commands.append(0)

    prev_index = 0
    prev_img_data = None
    if prev_frame:
        prev_img_data: bytes = prev_frame.convert("L").tobytes()
    img_data: bytes = img.convert("L").tobytes()

    # what if we just abstract the X and Y away and work
    # in the decoder's native language instead?
    # using X and Y only when necessary (MOVE cmd)
    for index in range(img.size[0] * img.size[1]):
        bg_clr = 0 if not prev_frame else prev_img_data[index]
        clr = img_data[index]
        if clr != bg_clr:
            # move us to the right position and change the colour
            # size, math opt: use INC_BY wherever possible
            if 256 > index - prev_index > 0:
                if index - prev_index == 1:
                    # size opt: use INC instead
                    commands.append(CMD_INC)
                else:
                    commands.append(CMD_INC_BY)
                    commands.append(index - prev_index)
            elif index - prev_index != 0:
                x = index % img.size[0]
                y = (index - x) // img.size[0]
                commands.append(CMD_MOVE)
                commands += list(struct.pack("HH", x, y))
            # change the colour
            # size opt: use INVERT if possible
            if bg_clr == 0xFF - clr:
                commands.append(CMD_INVERT)
            else:
                commands.append(CMD_DRAW)
                commands.append(clr)
            prev_index = index + 1  # because the draw operation shifted the index

    # TODO: optimization with FILL_DATA cmd
    # though i don't think i'll need it
    return commands


def imgs_to_video(path: str, frame_count: int, width: int, height: int, fps: int):
    prev_frame = None
    frames = []
    for i in tqdm.tqdm(range(frame_count), total=frame_count, desc="processing frames"):
        frame = Image.open(f"{path}/{(i + 1):04d}.png")
        frames.append(img_to_frame(prev_frame, frame))
        prev_frame = frame
    print(f"[info] creating video object...")
    video_obj = b"OMAVIDEO" + struct.pack("HHBH", width, height, fps, frame_count)
    for frame in frames:
        video_obj += struct.pack("I", len(frame)) + bytes(frame)

    return video_obj


if __name__ == "__main__":
    BAD_APPLE_FRAMES = 6572
    video = imgs_to_video("frames", BAD_APPLE_FRAMES, 480, 360, 30)
    with open("test.bin", "wb+") as file:
        file.write(video)
