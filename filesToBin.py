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
# repeat previous command of size S N times - 0x30 [S, 8-bit word] [N, 8-bit word]

CMD_MOVE = 0x01
CMD_INC_BY = 0x02
CMD_INC = 0x03

CMD_DRAW = 0x10
CMD_INVERT = 0x11
CMD_FILL = 0x12
CMD_FILL_DATA = 0x13

CMD_REPEAT = 0x30


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


class CmdRepeatData:
    def __init__(self, commands: list[int]):
        self._commands: list[int] = commands
        self.cmd: int = -1
        self.cmd_args: list[int] = []
        self.count: int = 0
        self.have_replaced: bool = False

    def add_command(self, cmd: int, *args: list[int]):
        if self.cmd == cmd and self.cmd_args == args:
            self.count += 1
            if self.count >= 256:
                self.count = 1
                self.have_replaced = False
                self._commands.append(cmd)
                for arg in args:
                    self._commands.append(arg)
            if self.have_replaced:
                # increment count
                self._commands[-1] = self.count - 1
            else:
                if self.count >= 2:
                    self._commands.append(CMD_REPEAT)
                    self._commands.append(len(args) + 1)
                    self._commands.append(self.count - 1)
                    self.have_replaced = True
        else:
            self.cmd = cmd
            self.cmd_args = args
            self.count = 1
            self.have_replaced = False
            self._commands.append(cmd)
            for arg in args:
                self._commands.append(arg)


def img_to_frame(prev_frame: Image.Image | None, img: Image.Image):
    commands = []

    repeat_data = CmdRepeatData(commands)
    add_command = repeat_data.add_command  # helper shortcut

    if not prev_frame:
        # fill with black
        # TODO: optimize by getting the most prelevant colour in the frame
        add_command(CMD_FILL, 0)

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
                    add_command(CMD_INC)
                else:
                    add_command(CMD_INC_BY, index - prev_index)
            elif index - prev_index != 0:
                x = index % img.size[0]
                y = (index - x) // img.size[0]
                add_command(CMD_MOVE, *list(struct.pack("HH", x, y)))
            # change the colour
            # size opt: use INVERT if possible
            if bg_clr == 0xFF - clr:
                add_command(CMD_INVERT)
            else:
                add_command(CMD_DRAW, clr)
            prev_index = index + 1  # because the draw operation shifted the index

    # TODO: optimization with FILL_DATA cmd
    # though i don't think i'll need it
    return bytes(commands)


def imgs_to_video(
    fp, path: str, frame_count: int, width: int, height: int, fps: int, is_1bit=False
):
    prev_frame = None
    fp.write(b"OMAVIDEO" + struct.pack("HHBH", width, height, fps, frame_count))
    for i in tqdm.tqdm(range(frame_count), total=frame_count, desc="processing frames"):
        frame = Image.open(f"{path}/{(i + 1):04d}.png")
        if is_1bit:
            frame = frame.convert("1")
        frame_data = img_to_frame(prev_frame, frame)
        fp.write(struct.pack("I", len(frame_data)) + frame_data)
        prev_frame = frame


if __name__ == "__main__":
    """
    settings for bad apple:
    frames = 6572
    res = 480, 360
    fps = 30
    is_1bit = True
    """
    BAD_APPLE_FRAMES = 6572
    with open("test.bin", "wb+") as file:
        imgs_to_video(file, "frames", 30 * 30, 480, 360, 30, False)
