#include <TimeUtils/Duration.hpp>

#include <SDL2/SDL.h>

#include <cassert>
#include <iostream>

using TimeUtils::Duration;


enum VideoMode
{
  Windowed,
  WindowedBorderless,
  Fullscreen,
  FullscreenBorderless,
};

enum DeltaTimeMode
{
  Raw,
  Average,
  FixedFloor,
  FixedRound,
  FixedInterpolated,
};

enum DeltaTimeGraphMode
{
  Hidden,
  Absolute,
  Relative,
};


double
deltaTimeAverage(
  Duration* deltaTimeBuffer,
  size_t bufferHead,
  size_t bufferSize );

void
drawTimings(
  SDL_Renderer*,
  DeltaTimeGraphMode,
  Duration* deltaTimeBuffer,
  size_t bufferHead,
  size_t bufferSize,
  double yMin,
  double yMax );


int
main(
  int argc,
  char* argv[] )
{
  std::cout << "cmd args:\n";

  for ( size_t i {1}; i < argc; ++i )
    std::cout << "  " << argv[i] << "\n";

  std::cout << "\n";


  const char* defaultArgs[]
  {
    "",

//    monitor index
    "0",

//    window size
    "1600", "900",

//    video mode
    "0",

//    vsync
    "1",

//    tick rate
    "0",

//    sleep between ticks
    "1",

//    poll events
    "1",

//    delta time mode
    "0",

//    delta time buffer size
    "300",

//    delta time graph mode
    "1",

//    subpixel rendering
    "1",

//    rect size
    "64", "1080",

//    rect speed
    "640", "0",
  };

  auto params = static_cast <const char* const*> (argv);

  if ( argc != std::size(defaultArgs) )
    params = defaultArgs;


  int monitorIndex = std::stoi(params[1]);

  auto windowWidth = std::stoi(params[2]);
  auto windowHeight = std::stoi(params[3]);

  int videoMode = std::stoi(params[4]);

  bool useVSync = std::stoi(params[5]);
  bool sleepBetweenTicks = std::stoi(params[6]);

  double tickRate = std::stod(params[7]);

  bool pollEvents = std::stoi(params[8]);

  int deltaTimeMode = std::stoi(params[9]);
  int deltaTimeBufferSize = std::stoi(params[10]);
  int deltaTimeGraphMode = std::stoi(params[11]);

  bool useSubpixelRendering = std::stoi(params[12]);

  double rectWidth = std::stoi(params[13]);
  double rectHeight = std::stoi(params[14]);

  double rectSpeedX = std::stoi(params[15]);
  double rectSpeedY = std::stoi(params[16]);


  if ( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 )
  {
    std::cout << "Failed to initialize SDL video subsystem. SDL Error: " << SDL_GetError() << "\n";
    return 1;
  }


  if ( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) == false )
    std::cout << "\nWarning: Failed to enable nearest texture sampling\n";

  if ( SDL_SetHint( SDL_HINT_RENDER_VSYNC, std::to_string(useVSync).c_str() ) == false )
    std::cout << "Warning: Failed to set V-Sync\n";


  int windowFlags =
    SDL_WINDOW_INPUT_FOCUS |
    SDL_WINDOW_MOUSE_FOCUS |
    0;

  switch (videoMode)
  {
    case VideoMode::Windowed:
      break;

    case VideoMode::WindowedBorderless:
      windowFlags |= SDL_WINDOW_BORDERLESS;
      break;

    case VideoMode::Fullscreen:
      windowFlags |= SDL_WINDOW_FULLSCREEN;
      break;

    case VideoMode::FullscreenBorderless:
      windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
      break;

    default:
      std::cout << "Unknown video mode " << videoMode << "\n";
      return 1;
  }


  auto window = SDL_CreateWindow(
    "Timing Demo",
    SDL_WINDOWPOS_CENTERED_DISPLAY(monitorIndex),
    SDL_WINDOWPOS_CENTERED_DISPLAY(monitorIndex),
    windowWidth, windowHeight,
    windowFlags );

  if ( window == nullptr )
  {
    std::cout << "Failed to create window. SDL Error: " << SDL_GetError() << "\n";
    return 1;
  }

  if ( videoMode == VideoMode::FullscreenBorderless )
  {
    int oldWindowWidth = windowWidth;
    int oldWindowHeight = windowHeight;

    SDL_GetWindowSize(
      window, &windowWidth, &windowHeight );

    auto resizedFactorX = 1.0 * windowWidth / oldWindowWidth;
    auto resizedFactorY = 1.0 * windowHeight / oldWindowHeight;

    rectWidth *= resizedFactorX;
    rectHeight *= resizedFactorY;
    rectSpeedX *= resizedFactorX;
    rectSpeedY *= resizedFactorY;
  }


  auto renderer = SDL_CreateRenderer(
    window, -1,
    SDL_RENDERER_ACCELERATED );

  if ( renderer == nullptr )
  {
    std::cout << "Failed to create renderer. SDL Error: " << SDL_GetError() << "\n";
    return 1;
  }


  if ( SDL_RenderSetVSync(renderer, useVSync) != 0 )
    std::cout << "Warning: Failed to set renderer V-Sync\n";


  SDL_DisplayMode windowMode;
  SDL_GetWindowDisplayMode(window, &windowMode);
  std::cout << "Window refresh rate: " << windowMode.refresh_rate << "\n";

  const auto refreshInterval = 1.0 / windowMode.refresh_rate;


  auto deltaTimeBuffer = new Duration[deltaTimeBufferSize];
  size_t bufferHead {};


  Duration tickInterval {};

  if ( tickRate > 0 )
    tickInterval = 1.0 / tickRate;


  SDL_RenderPresent(renderer);

  auto timePrevious = TimeUtils::Now();
  auto tickPrevious = timePrevious;


  double rectX {};
  double rectY {};
  double prevRectX {};
  double prevRectY {};


  bool isExiting {};

  while ( isExiting == false )
  {
    if ( useVSync == false && sleepBetweenTicks == true )
      TimeUtils::SleepUntil(tickPrevious + tickInterval);

    const auto currentTime = TimeUtils::Now();

    const auto elapsedTime = currentTime - timePrevious;

    double deltaTime = static_cast <double> (elapsedTime);

    timePrevious = currentTime;


    deltaTimeBuffer[bufferHead] = elapsedTime;
    bufferHead = ++bufferHead % deltaTimeBufferSize;


    if ( pollEvents == true )
    {
      SDL_Event windowEvent {};

      while ( SDL_PollEvent(&windowEvent) != 0 )
      {
        if (  windowEvent.type == SDL_QUIT ||
              windowEvent.type == SDL_KEYDOWN )
        {
          isExiting = true;
          break;
        }
      }
    }


    uint32_t ticks = 0;

    switch (deltaTimeMode)
    {
      case DeltaTimeMode::Raw:
      case DeltaTimeMode::Average:
      {
        ticks = 1;

        if ( deltaTimeMode == DeltaTimeMode::Average )
          deltaTime = deltaTimeAverage(
            deltaTimeBuffer, bufferHead,
            deltaTimeBufferSize );


        if ( tickInterval == Duration{} )
          break;

        for ( ; tickPrevious + tickInterval <= currentTime;
              tickPrevious += tickInterval )
          ;

        break;
      }

      case DeltaTimeMode::FixedFloor:
      case DeltaTimeMode::FixedRound:
      case DeltaTimeMode::FixedInterpolated:
      {
        if ( tickInterval == Duration{} )
        {
          ticks = 1;
          break;
        }

        auto tickMargin = tickInterval;

        if ( deltaTimeMode == DeltaTimeMode::FixedRound )
          tickMargin = 0.5 * static_cast <double> (tickInterval);

        for ( ; tickPrevious + tickMargin <= currentTime;
              tickPrevious += tickInterval )
          ++ticks;

        deltaTime = ticks * static_cast <double> (tickInterval);

        break;
      }

      default:
        break;
    }


    if ( ticks == 0 )
//      continue;
      ;
    else
    {
      if ( ticks > 1 )
        ;

      prevRectX = rectX;
      prevRectY = rectY;

      auto deltaX = rectSpeedX * deltaTime;
      auto deltaY = rectSpeedY * deltaTime;

      rectX += deltaX;
      rectY += deltaY;

      if ( rectSpeedX != 0 )
      {
        if ( rectX > windowWidth )
        {
          rectX -= windowWidth;
          prevRectX = rectX - deltaX;
        }
        else if ( rectX < 0 )
        {
          rectX += windowWidth;
          prevRectX = rectX + deltaX;
        }
      }

      if ( rectSpeedY != 0 )
      {
        if ( rectY > windowHeight )
        {
          rectY -= windowHeight;
          prevRectY = rectY - deltaY;
        }
        else if ( rectY < 0 )
        {
          rectY += windowHeight;
          prevRectY = rectY + deltaY;
        }
      }

      if ( tickInterval == Duration{} ||
           deltaTimeMode != DeltaTimeMode::FixedInterpolated )
      {
        prevRectX = rectX;
        prevRectY = rectY;
      }
    }


    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear(renderer);


    SDL_FRect frect
    {
      prevRectX, prevRectY,
      rectWidth, rectHeight,
    };

    if ( tickInterval != Duration{} &&
         deltaTimeMode == DeltaTimeMode::FixedInterpolated )
    {
      auto tickStart = tickPrevious;

      if ( currentTime < tickPrevious )
        tickStart -= tickInterval;

      auto timeInTick =
        static_cast <double> (currentTime - tickStart);

      frect.x += rectSpeedX * timeInTick;
      frect.y += rectSpeedY * timeInTick;
    }

    SDL_Rect rect
    {
      frect.x, frect.y,
      frect.w, frect.h,
    };


    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );

    if ( useSubpixelRendering == true )
      SDL_RenderFillRectF(renderer, &frect);
    else
      SDL_RenderFillRect(renderer, &rect);


    if ( rectSpeedX != 0 )
    {
      frect.x += 0.5 * windowWidth;
      rect.x = frect.x;
    }

    if ( rectSpeedY != 0 )
    {
      frect.y += 0.5 * windowHeight;
      rect.y = frect.y;
    }

    if ( useSubpixelRendering == true )
      SDL_RenderFillRectF(renderer, &frect);
    else
      SDL_RenderFillRect(renderer, &rect);


    if ( rectSpeedX != 0 )
    {
      frect.x -= windowWidth;
      rect.x = frect.x;
    }

    if ( rectSpeedY != 0 )
    {
      frect.y -= windowHeight;
      rect.y = frect.y;
    }

    if ( useSubpixelRendering == true )
      SDL_RenderFillRectF(renderer, &frect);
    else
      SDL_RenderFillRect(renderer, &rect);


    drawTimings(
      renderer,
      static_cast <DeltaTimeGraphMode> (deltaTimeGraphMode),
      deltaTimeBuffer,
      bufferHead, deltaTimeBufferSize,
      windowHeight,
      windowHeight / 2 );


    SDL_RenderPresent(renderer);
  }

  delete[] deltaTimeBuffer;

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}


double
deltaTimeAverage(
  Duration* deltaTimeBuffer,
  size_t bufferHead,
  size_t bufferSize )
{
  auto validValueCount = bufferSize;

  Duration deltaTime {};

  for ( size_t i = bufferHead + 1; ; ++i )
  {
    if ( i == bufferSize )
      i -= bufferSize;

    auto dtCurrent = deltaTimeBuffer[i];

    if ( dtCurrent != Duration{} )
      deltaTime += dtCurrent;
    else
      --validValueCount;

    if ( i == bufferHead )
      break;
  }

  return static_cast <double> (deltaTime) / validValueCount;
}

void
drawTimings(
  SDL_Renderer* renderer,
  DeltaTimeGraphMode graphMode,
  Duration* deltaTimeBuffer,
  size_t bufferHead,
  size_t bufferSize,
  double yMin,
  double yMax )
{
  Duration dtMin {};
  Duration dtMax {};


  switch (graphMode)
  {
    case DeltaTimeGraphMode::Hidden:
      return;

    case DeltaTimeGraphMode::Relative:
    {
      dtMin = { std::numeric_limits <int64_t>::max(), 0 };
      break;
    }

    default:
      break;
  }


  for ( size_t i = bufferHead; ; ++i )
  {
    if ( i == bufferSize )
      i -= bufferSize;

    auto dtCurrent = deltaTimeBuffer[i];

    if ( dtCurrent != Duration{} )
    {
      if ( dtCurrent < dtMin )
        dtMin = dtCurrent;

      if ( dtCurrent > dtMax )
        dtMax = dtCurrent;
    }

    if ( (i + 1) % bufferSize == bufferHead )
      break;
  }

  const auto dtRange =
    static_cast <double> (dtMax - dtMin);

  if ( dtRange <= 0.0 )
    return;

  SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );

  float yRange = yMin - yMax;

  size_t lineX {};

  for ( size_t i = bufferHead; ; ++i, ++lineX )
  {
    if ( i == bufferSize )
      i -= bufferSize;

    auto dtCurrent = deltaTimeBuffer[i];

    if ( dtCurrent != Duration{} )
    {
      auto dtNormalized =
        static_cast <double> (dtCurrent - dtMin) / dtRange;

      SDL_RenderDrawLineF(
        renderer,
        lineX, yMin,
        lineX, yMin - yRange * dtNormalized );
    }

    if ( (i + 1) % bufferSize == bufferHead )
      break;
  }
}
