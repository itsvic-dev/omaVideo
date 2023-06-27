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
# "OMFR" - magic header
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

    for x in range(img.size[0]):
        for y in range(img.size[1]):
            index = y * img.size[0] + x
            clr = img_data[index]
            idx_diff = index - prev_index

            bg_clr = 0
            if prev_img_data:
                bg_clr = prev_img_data[index]

            if clr != bg_clr:
                if 256 > idx_diff > 0:
                    # increment index
                    # no command optimization
                    if idx_diff == 1:
                        pass
                    else:
                        commands.append(CMD_INC_BY)
                        commands.append(idx_diff)
                elif idx_diff != 0:
                    # move to X,Y
                    commands.append(CMD_MOVE)
                    commands += list(struct.pack("HH", x, y))
                # invert opt
                if clr == (256 + clr - bg_clr) % 256:
                    commands.append(CMD_INVERT)
                else:
                    commands.append(CMD_DRAW)
                    commands.append(clr)
                prev_index = index

    # TODO: optimization with 0x13 command
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
        video_obj += b"OMFR" + struct.pack("I", len(frame)) + bytes(frame)

    return video_obj


if __name__ == "__main__":
    BAD_APPLE_FRAMES = 6572
    video = imgs_to_video("frames", 5 * 30, 480, 360, 30)
    with open("test.bin", "wb+") as file:
        file.write(video)
