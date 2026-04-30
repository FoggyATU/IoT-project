extends Control

var REFRESH_URL:String = "http://127.0.0.1:6768"
const DOWNLOAD_URL:String = "https://cool.foggydude.dev/?url={0}"

@onready var download:AwaitableHTTPRequest = $Downloader
@onready var player:AudioStreamPlayer = $Player
@onready var info_label:Label = $InfoLabel
@onready var refresh_timer: Timer = $RefreshTimer

var currently_playing:String = ""

#TODO LIST:
#Do a request every like 2secs to the Arduino to see what song to play
#Get name of the song played (maybe get it from the arduino)
#Cool awesome interface

func start():
	#REFRESH_URL = $ArudinoAddress.text.strip_edges()
	#get_audio_data()
	download_audio("KWoTyfPsqbE")
	
func get_audio_data():
	var resp := await download.async_request(REFRESH_URL)
	if !resp.success() or resp.status_err():
		push_error("Request failed.")
	
	var output = resp.body_as_string()
	var split = output.split('¬')
	var song_name = split[0]
	var song_id = split[1]
	
	if song_name != currently_playing:
		currently_playing = song_name
		info_label.text = "LOADING..."
		
		await download_audio(song_id)
		
		info_label.text = "Currently playing:\n%s"%[currently_playing]
	refresh_timer.start()

func download_audio(video_url:String):
	print("downloading audio")
	var resp := await download.async_request(DOWNLOAD_URL.format([video_url]))
	if !resp.success() or resp.status_err():
		push_error("Request failed.")

	load_audio(resp.bytes)

func load_audio(bytes:PackedByteArray):
	var audio := AudioStreamMP3.new()
	audio.data = bytes
	player.stream = audio
	player.play()
