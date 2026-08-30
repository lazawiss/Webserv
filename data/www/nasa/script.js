/* ============================================================
   CONFIGURATION — NASA info
   ============================================================ */
const NASA_API_URL = "https://api.nasa.gov/planetary/apod";
/* ============================================================ */

async function loadApod()
{
  const loading = document.getElementById("state-loading");
  const errorState = document.getElementById("state-error");
  const errorDetail = document.getElementById("error-detail");
  const content = document.getElementById("state-content");

  try
  {
    const url = `${NASA_API_URL}?api_key=${encodeURIComponent(NASA_API_KEY)}`;
    const response = await fetch(url);

    if (!response.ok)
    {
      if (response.status === 403)
        throw new Error("Invalid or missing API key.");
      if (response.status === 429)
        throw new Error("Too many requests — NASA's hourly quota was reached.");

      throw new Error(`NASA's server responded with status ${response.status}.`);
    }

    const data = await response.json();
    console.log("data: ", data);
    renderApod(data);

    loading.classList.add("hidden");
    content.classList.remove("hidden");

  } 
  catch (err)
  {
    console.error(err);
    errorDetail.textContent = err.message || "An unknown error occurred.";
    loading.classList.add("hidden");
    errorState.classList.remove("hidden");
  }
}

function renderApod(data) {
  const dateEl = document.getElementById("apod-date");
  const titleEl = document.getElementById("apod-title");
  const explanationEl = document.getElementById("apod-explanation");
  const copyrightEl = document.getElementById("apod-copyright");
  const imageEl = document.getElementById("apod-image");
  const videoEl = document.getElementById("apod-video");

  dateEl.textContent = data.date || "";
  titleEl.textContent = data.title || "Untitled";
  explanationEl.textContent = data.explanation || "";
  copyrightEl.textContent = data.copyright ? data.copyright : "NASA (public domain)";

  if (data.media_type === "video")
  {
    imageEl.classList.add("hidden");
    videoEl.classList.remove("hidden");
    videoEl.src = data.url;
  }
  else
  {
    videoEl.classList.add("hidden");
    imageEl.classList.remove("hidden");
    imageEl.src = data.hdurl || data.url;
    imageEl.alt = data.title || "NASA image of the day";
  }
}

function initStarfield()
{
  const canvas = document.getElementById("stars");

  // 2D drawing context for the canvas
  const ctx = canvas.getContext("2d");
  let stars = [];

  function resize()
  {
    // Match canvas size to the viewport
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;

    // Number of stars scales with screen area (higher divisor = fewer stars)
    const count = Math.floor((canvas.width * canvas.height) / 18000);

    // Generate each star with a random position, radius, and twinkle rhythm
    stars = Array.from({ length: count }, () => ({
      x: Math.random() * canvas.width,
      y: Math.random() * canvas.height,
      r: Math.random() * 1.2 + 0.2,         // radius between 0.2 and 1.4 px
      phase: Math.random() * Math.PI * 2,   // random starting point in the twinkle cycle
      speed: Math.random() * 0.015 + 0.005, // twinkle speed (radians per ms)
    }));
  }

  function draw(time)
  {
    // Clear the previous frame
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    stars.forEach(star => {
      // Oscillate opacity between 0.4 and 0.8 to create a twinkling effect
      const opacity = 0.4 + 0.4 * Math.sin(time * star.speed + star.phase);
      ctx.beginPath();
      ctx.arc(star.x, star.y, star.r, 0, Math.PI * 2);
      ctx.fillStyle = `rgba(237, 238, 245, ${opacity})`;
      ctx.fill();
    });

    // Schedule the next frame
    requestAnimationFrame(draw);
  }

  resize();
  window.addEventListener("resize", resize); // Regenerate stars on window resize
  requestAnimationFrame(draw);
}

initStarfield();
loadApod();