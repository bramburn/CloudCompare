import clsx from 'clsx';
import Heading from '@theme/Heading';
import Link from '@docusaurus/Link';
import styles from './styles.module.css';

const FeatureList = [
  {
    title: 'Octree-accelerated comparison',
    icon: (
      <svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
        <defs>
          <linearGradient id="cmp-a" x1="0" y1="0" x2="64" y2="64" gradientUnits="userSpaceOnUse">
            <stop stopColor="#60a5fa" />
            <stop offset="1" stopColor="#2563eb" />
          </linearGradient>
        </defs>
        <g fill="url(#cmp-a)">
          <circle cx="18" cy="46" r="2.4" />
          <circle cx="24" cy="38" r="2.4" />
          <circle cx="32" cy="34" r="2.4" />
          <circle cx="28" cy="28" r="2.4" />
          <circle cx="36" cy="42" r="2.4" />
          <circle cx="44" cy="36" r="2.4" />
          <circle cx="40" cy="22" r="2.4" />
          <circle cx="48" cy="28" r="2.4" />
          <circle cx="32" cy="48" r="2.4" />
          <circle cx="50" cy="40" r="2.4" />
        </g>
        <g fill="#f59e0b" opacity="0.85">
          <circle cx="16" cy="36" r="2.4" />
          <circle cx="22" cy="28" r="2.4" />
          <circle cx="28" cy="44" r="2.4" />
          <circle cx="38" cy="30" r="2.4" />
          <circle cx="46" cy="20" r="2.4" />
          <circle cx="42" cy="38" r="2.4" />
          <circle cx="52" cy="34" r="2.4" />
          <circle cx="34" cy="18" r="2.4" />
        </g>
      </svg>
    ),
    description: (
      <>
        Compare two 3D point clouds (or a cloud and a mesh) using the
        octree-based{' '}
        <a
          href="https://www.cloudcompare.org/doc/wiki/index.php?title=Cloud-to-Cloud_Distance">
          Cloud-to-Cloud Distance
        </a>{' '}
        tool — designed for the difference maps you produce from a laser
        scanner and a reference mesh.
      </>
    ),
  },
  {
    title: '120M+ points in 2 GB',
    icon: (
      <svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
        <g fill="#2563eb">
          {[0, 1, 2, 3, 4, 5, 6].map((i) =>
            [0, 1, 2, 3, 4, 5, 6].map((j) => (
              <circle key={`${i}-${j}`} cx={6 + i * 9} cy={6 + j * 9} r="1.4" />
            ))
          )}
        </g>
        <g fill="#60a5fa" opacity="0.5">
          {[0, 1, 2, 3, 4, 5, 6].map((i) =>
            [0, 1, 2, 3, 4, 5].map((j) => (
              <circle key={`s-${i}-${j}`} cx={10 + i * 9} cy={10 + j * 9} r="1.4" />
            ))
          )}
        </g>
      </svg>
    ),
    description: (
      <>
        The original design target. CloudCompare handles dense terrestrial and
        mobile laser scans on modest hardware by streaming chunks through the
        octree rather than loading the whole cloud into memory.
      </>
    ),
  },
  {
    title: 'Plugin-based I/O & analysis',
    icon: (
      <svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
        <g fill="none" stroke="#2563eb" strokeWidth="2.5" strokeLinejoin="round" strokeLinecap="round">
          <path d="M8 10h18v10a4 4 0 0 0 8 0V10h22v18h-10a4 4 0 0 0 0 8h10v18H34V44a4 4 0 0 0-8 0v10H8V36h10a4 4 0 0 0 0-8H8Z" />
        </g>
        <g fill="#60a5fa" opacity="0.4">
          <rect x="16" y="20" width="32" height="28" rx="3" />
        </g>
      </svg>
    ),
    description: (
      <>
        File formats (LAS, E57, PDAL, FBX, OBJ, PLY, …) and analysis tools
        (M3C2, CSF, RANSAC, Poisson) ship as <code>Standard</code>,{' '}
        <code>I/O</code>, and <code>GL</code> plugins. Drop in the ones you
        need, leave the rest off.
      </>
    ),
  },
  {
    title: 'Cross-platform desktop app',
    icon: (
      <svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
        <g fill="#2563eb">
          <rect x="8" y="10" width="22" height="22" rx="2" />
          <rect x="34" y="10" width="22" height="22" rx="2" />
          <rect x="8" y="36" width="48" height="20" rx="2" />
        </g>
        <g fill="#dbeafe">
          <path d="M14 14h4v4h-4zM22 14h4v4h-4zM14 22h4v4h-4zM22 22h4v4h-4z" />
          <circle cx="45" cy="22" r="3" />
          <ellipse cx="45" cy="22" rx="3" ry="1.4" />
        </g>
        <g fill="#93c5fd">
          <circle cx="32" cy="46" r="2.5" />
          <circle cx="42" cy="50" r="1.8" />
        </g>
      </svg>
    ),
    description: (
      <>
        <code>CloudCompare.exe</code> on Windows, .AppImage / .deb on Linux,
        <code>CloudCompare.app</code> on macOS (community-supported, not
        CI-validated since 2026-08-24). Same Qt 6 UI, same code path, same
        plugin folder. Build scripts for each platform live in this docs
        site.
      </>
    ),
  },
];

function Feature({title, icon, description}) {
  return (
    <div className={clsx('col col--6', styles.featureCol)}>
      <div className={styles.featureCard}>
        <div className={styles.featureIcon}>{icon}</div>
        <div className={styles.featureBody}>
          <Heading as="h3">{title}</Heading>
          <p>{description}</p>
        </div>
      </div>
    </div>
  );
}

function ForkSection() {
  return (
    <section className={styles.forkSection}>
      <div className="container">
        <div className={styles.forkHeader}>
          <span className={styles.forkEyebrow}>Why this fork</span>
          <Heading as="h2">A pinned, opinionated build for surveying workflows</Heading>
          <p>
            The upstream ships every plugin by default and asks you to install
            vcpkg, OpenCASCADE, PDAL, PCL, and the FBX SDK. The fork targets a
            specific hardware profile (Windows Server 2019, VS 2022, 2&nbsp;GB
            dense scans from a terrestrial laser scanner) and ships a
            self-contained Windows bundle that just runs.
          </p>
        </div>
        <div className="row">
          <div className={clsx('col col--4', styles.forkCol)}>
            <div className={styles.forkCard}>
              <div className={styles.forkCardLabel}>Toolchain</div>
              <p>
                Pinned to CMake 4.3, Ninja, Qt 6.8.3, MSVC 14.44. The
                <code>C:\dev\tools\</code> wrapper scripts reproduce the
                exact GitHub Actions build locally.
              </p>
            </div>
          </div>
          <div className={clsx('col col--4', styles.forkCol)}>
            <div className={styles.forkCard}>
              <div className={styles.forkCardLabel}>Plugin set</div>
              <p>
                18 self-contained plugins enabled by default. The 20+ that
                need external deps are off by default; the vcpkg recipe to
                enable each one is documented.
              </p>
            </div>
          </div>
          <div className={clsx('col col--4', styles.forkCol)}>
            <div className={styles.forkCard}>
              <div className={styles.forkCardLabel}>Bundle</div>
              <p>
                <code>deployqt\CloudCompare.exe</code> is a 70&nbsp;MB
                self-contained folder — Qt 6 runtime + plugin DLLs bundled,
                no PATH manipulation. Copy to any Windows box and run.
              </p>
            </div>
          </div>
        </div>
        <div className={styles.forkCta}>
          <Link
            className="button button--primary button--lg"
            to="/docs/getting-started/overview">
            Start with the build cookbook →
          </Link>
        </div>
      </div>
    </section>
  );
}

export default function HomepageFeatures() {
  return (
    <>
      <section className={styles.features}>
        <div className="container">
          <div className={styles.featuresHeader}>
            <Heading as="h2">What CloudCompare does</Heading>
            <p>
              Three pillars of the original design, plus a fourth that
              matters for a surveying-company build.
            </p>
          </div>
          <div className="row">
            {FeatureList.map((props, idx) => (
              <Feature key={idx} {...props} />
            ))}
          </div>
        </div>
      </section>
      <ForkSection />
    </>
  );
}
